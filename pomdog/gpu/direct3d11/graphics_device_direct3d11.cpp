// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/direct3d11/graphics_device_direct3d11.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/buffer_bind_mode.h"
#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/gpu/backends/command_list_immediate.h"
#include "pomdog/gpu/backends/shader_bytecode.h"
#include "pomdog/gpu/backends/shader_compile_options.h"
#include "pomdog/gpu/backends/texture_helper.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/direct3d11/buffer_direct3d11.h"
#include "pomdog/gpu/direct3d11/depth_stencil_buffer_direct3d11.h"
#include "pomdog/gpu/direct3d11/effect_reflection_direct3d11.h"
#include "pomdog/gpu/direct3d11/pipeline_state_direct3d11.h"
#include "pomdog/gpu/direct3d11/render_target2d_direct3d11.h"
#include "pomdog/gpu/direct3d11/sampler_state_direct3d11.h"
#include "pomdog/gpu/direct3d11/shader_direct3d11.h"
#include "pomdog/gpu/direct3d11/texture2d_direct3d11.h"
#include "pomdog/gpu/index_buffer.h"
#include "pomdog/gpu/pipeline_descriptor.h"
#include "pomdog/gpu/shader_language.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/logging/log.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {
namespace {

using Microsoft::WRL::ComPtr;

[[nodiscard]] std::tuple<ComPtr<ID3D11InfoQueue>, std::unique_ptr<Error>>
buildInfoQueue(unsafe_ptr<ID3D11Device> nativeDevice) noexcept
{
    POMDOG_ASSERT(nativeDevice != nullptr);

    ComPtr<ID3D11InfoQueue> infoQueue = nullptr;
    if (auto hr = nativeDevice->QueryInterface(IID_PPV_ARGS(&infoQueue)); FAILED(hr)) {
        if (hr == E_NOINTERFACE) {
            return std::make_tuple(nullptr, nullptr);
        }
        return std::make_tuple(nullptr, errors::make("failed to get ID3D11InfoQueue"));
    }

    if (infoQueue == nullptr) {
        return std::make_tuple(nullptr, nullptr);
    }

    infoQueue->ClearRetrievalFilter();
    infoQueue->ClearStorageFilter();

    D3D11_INFO_QUEUE_FILTER filter;
    ZeroMemory(&filter, sizeof(filter));

    std::array<D3D11_MESSAGE_SEVERITY, 4> severityList = {
        D3D11_MESSAGE_SEVERITY_CORRUPTION,
        D3D11_MESSAGE_SEVERITY_ERROR,
        D3D11_MESSAGE_SEVERITY_WARNING,
        D3D11_MESSAGE_SEVERITY_INFO,
    };

    filter.DenyList.pSeverityList = severityList.data();
    filter.DenyList.NumSeverities = static_cast<UINT>(severityList.size());

    if (auto hr = infoQueue->AddStorageFilterEntries(&filter); FAILED(hr)) {
        return std::make_tuple(nullptr, errors::make("AddStorageFilterEntries() failed"));
    }
    if (auto hr = infoQueue->AddRetrievalFilterEntries(&filter); FAILED(hr)) {
        return std::make_tuple(nullptr, errors::make("AddRetrievalFilterEntries() failed"));
    }

    return std::make_tuple(std::move(infoQueue), nullptr);
}

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
[[nodiscard]] std::unique_ptr<Error>
checkError(ID3D11InfoQueue* infoQueue) noexcept
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(infoQueue != nullptr);
#endif
    if (infoQueue == nullptr) {
        return nullptr;
    }

    const auto storedMessageCount = infoQueue->GetNumStoredMessages();

    std::string message;
    for (UINT64 i = 0; i < storedMessageCount; ++i) {
        SIZE_T messageByteLength = 0;
        infoQueue->GetMessage(i, nullptr, &messageByteLength);

        if (messageByteLength <= 0) {
            continue;
        }

        std::vector<D3D11_MESSAGE> messageBuffer(messageByteLength);
        infoQueue->GetMessage(i, messageBuffer.data(), &messageByteLength);
        message += messageBuffer.data()->pDescription;
        message += "\n";
    }

    if (!message.empty()) {
        return errors::make(std::move(message));
    }

    return nullptr;
}
#endif

[[nodiscard]] D3D11_BIND_FLAG
toBindFlag(BufferBindMode bindMode) noexcept
{
    switch (bindMode) {
    case BufferBindMode::ConstantBuffer:
        return D3D11_BIND_CONSTANT_BUFFER;
    case BufferBindMode::IndexBuffer:
        return D3D11_BIND_INDEX_BUFFER;
    case BufferBindMode::VertexBuffer:
        return D3D11_BIND_VERTEX_BUFFER;
    }
    return D3D11_BIND_VERTEX_BUFFER;
}

[[nodiscard]] std::unique_ptr<Error>
buildDevice(
    AdapterManager& adapters,
    Microsoft::WRL::ComPtr<ID3D11Device3>& device,
    D3D_DRIVER_TYPE& driverType,
    D3D_FEATURE_LEVEL& featureLevel) noexcept
{
    POMDOG_ASSERT(!device);

    UINT createDeviceFlags = 0;
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    // NOTE: supported Direct2D1
    //createDeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    constexpr std::array<D3D_DRIVER_TYPE, 3> driverTypes = {{
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    }};

    constexpr std::array<D3D_FEATURE_LEVEL, 7> featureLevels = {{
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    }};

    HRESULT hr = S_OK;

    ComPtr<ID3D11Device> d3d11Device;

    auto adapter = adapters.getActiveAdapter();
    for (auto& type : driverTypes) {
        driverType = type;
        hr = D3D11CreateDevice(
            adapter,
            (nullptr != adapter ? D3D_DRIVER_TYPE_UNKNOWN : driverType),
            nullptr,
            createDeviceFlags,
            featureLevels.data(),
            static_cast<UINT>(featureLevels.size()),
            D3D11_SDK_VERSION,
            &d3d11Device,
            &featureLevel,
            nullptr);

        if (SUCCEEDED(hr)) {
            break;
        }
    }

    if (FAILED(hr)) {
        return errors::make("failed to create ID3D11Device");
    }

    // NOTE: Create ID3D11Device3 from ID3D11Device.
    if (hr = d3d11Device->QueryInterface(__uuidof(ID3D11Device3), &device); FAILED(hr)) {
        return errors::make("failed to create ID3D11Device3 from ID3D11Device");
    }
    d3d11Device.Reset();

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    {
        switch (driverType) {
        case D3D_DRIVER_TYPE_HARDWARE:
            Log::Internal("Direct3D11 DriverType: Hardware Device.");
            break;
        case D3D_DRIVER_TYPE_WARP:
            Log::Internal("Direct3D11 DriverType: Warp Device.");
            break;
        case D3D_DRIVER_TYPE_REFERENCE:
            Log::Internal("Direct3D11 DriverType: Reference Device.");
            break;
        default:
            break;
        }
    }
    {
        D3D11_FEATURE_DATA_D3D11_OPTIONS d3d11Options;
        hr = device->CheckFeatureSupport(
            D3D11_FEATURE_D3D11_OPTIONS,
            &d3d11Options,
            sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS));

        if (FAILED(hr)) {
            return errors::make("CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS) failed");
        }

        Log::Internal("MapNoOverwriteOnDynamicBufferSRV: " +
                      std::to_string(d3d11Options.MapNoOverwriteOnDynamicBufferSRV));
        Log::Internal("MapNoOverwriteOnDynamicConstantBuffer: " +
                      std::to_string(d3d11Options.MapNoOverwriteOnDynamicConstantBuffer));
    }
#endif
    return nullptr;
}

} // namespace

std::unique_ptr<Error>
AdapterManager::enumAdapters() noexcept
{
    Microsoft::WRL::ComPtr<IDXGIFactory1> dxgiFactory;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

    if (FAILED(hr)) {
        return errors::make("CreateDXGIFactory1() failed");
    }

    UINT i = 0;
    Microsoft::WRL::ComPtr<IDXGIAdapter1> newAdapter;
    while ((hr = dxgiFactory->EnumAdapters1(i, &newAdapter)) != DXGI_ERROR_NOT_FOUND) {
        if (FAILED(hr)) {
            return errors::make("failed to enumerate adapters");
        }

        adapters_.push_back(newAdapter);
        newAdapter.Reset();
        ++i;
    }

    if (adapters_.empty()) {
        return errors::make("List of DXGI adapters is empty");
    }

    POMDOG_ASSERT(!adapters_.empty());
    activeAdapter_ = adapters_.front();
    return nullptr;
}

void AdapterManager::clear()
{
    activeAdapter_.Reset();

    for (auto& adapter : adapters_) {
        adapter.Reset();
    }
    adapters_.clear();
}

IDXGIAdapter1*
AdapterManager::getActiveAdapter() const
{
    POMDOG_ASSERT(activeAdapter_);
    return activeAdapter_.Get();
}

std::tuple<Microsoft::WRL::ComPtr<IDXGIFactory1>, std::unique_ptr<Error>>
AdapterManager::getFactory() noexcept
{
    POMDOG_ASSERT(activeAdapter_ != nullptr);

    ComPtr<IDXGIFactory1> dxgiFactory;
    if (auto hr = activeAdapter_->GetParent(IID_PPV_ARGS(&dxgiFactory)); FAILED(hr)) {
        return std::make_tuple(nullptr, errors::make("failed to get DXGIFactory1"));
    }

    return std::make_tuple(std::move(dxgiFactory), nullptr);
}

std::unique_ptr<Error>
GraphicsDeviceDirect3D11::initialize(const PresentationParameters& presentationParametersIn) noexcept
{
    driverType_ = D3D_DRIVER_TYPE_NULL;
    featureLevel_ = D3D_FEATURE_LEVEL_11_1;
    presentationParameters_ = presentationParametersIn;

    if (auto err = adapters_.enumAdapters(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to enumerate adapters");
    }

    if (auto err = buildDevice(adapters_, device_, driverType_, featureLevel_); err != nullptr) {
        return errors::wrap(std::move(err), "buildDevice() failed");
    }

    std::unique_ptr<Error> err;
    std::tie(infoQueue_, err) = buildInfoQueue(device_.Get());
    if (err != nullptr) {
        return errors::wrap(std::move(err), "buildInfoQueue() failed");
    }

    return nullptr;
}

GraphicsDeviceDirect3D11::~GraphicsDeviceDirect3D11() = default;

ShaderLanguage
GraphicsDeviceDirect3D11::getSupportedLanguage() const noexcept
{
    return ShaderLanguage::HLSL;
}

PresentationParameters
GraphicsDeviceDirect3D11::getPresentationParameters() const noexcept
{
    return presentationParameters_;
}

std::tuple<std::shared_ptr<CommandList>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::createCommandList() noexcept
{
    auto commandList = std::make_shared<CommandListImmediate>();
    return std::make_tuple(std::move(commandList), nullptr);
}

std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::createVertexBuffer(
    const void* vertices,
    std::size_t vertexCount,
    std::size_t strideBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(vertices != nullptr);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(strideBytes > 0);

    const auto sizeInBytes = vertexCount * strideBytes;

    auto nativeBuffer = std::make_unique<BufferDirect3D11>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->initialize(
            device_.Get(),
            vertices,
            sizeInBytes,
            bufferUsage,
            toBindFlag(BufferBindMode::VertexBuffer));
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    if (auto err = checkError(infoQueue_.Get()); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "CheckError() failed"));
    }
#endif

    auto vertexBuffer = std::make_shared<VertexBuffer>(
        std::move(nativeBuffer),
        vertexCount,
        strideBytes,
        bufferUsage);
    return std::make_tuple(std::move(vertexBuffer), nullptr);
}

std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::createVertexBuffer(
    std::size_t vertexCount,
    std::size_t strideBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(strideBytes > 0);

    const auto sizeInBytes = vertexCount * strideBytes;

    auto nativeBuffer = std::make_unique<BufferDirect3D11>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->initialize(
            device_.Get(),
            sizeInBytes,
            bufferUsage,
            toBindFlag(BufferBindMode::VertexBuffer));
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    if (auto err = checkError(infoQueue_.Get()); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "CheckError() failed"));
    }
#endif

    auto vertexBuffer = std::make_shared<VertexBuffer>(
        std::move(nativeBuffer),
        vertexCount,
        strideBytes,
        bufferUsage);
    return std::make_tuple(std::move(vertexBuffer), nullptr);
}

std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::createIndexBuffer(
    IndexFormat elementSize,
    const void* indices,
    std::size_t indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(indexCount > 0);

    const auto sizeInBytes = indexCount * detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);

    auto nativeBuffer = std::make_unique<BufferDirect3D11>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->initialize(
            device_.Get(),
            indices,
            sizeInBytes,
            bufferUsage,
            toBindFlag(BufferBindMode::IndexBuffer));
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    if (auto err = checkError(infoQueue_.Get()); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "CheckError() failed"));
    }
#endif

    auto indexBuffer = std::make_shared<IndexBuffer>(
        std::move(nativeBuffer),
        elementSize,
        indexCount,
        bufferUsage);
    return std::make_tuple(std::move(indexBuffer), nullptr);
}

std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::createIndexBuffer(
    IndexFormat elementSize,
    std::size_t indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(indexCount > 0);

    const auto sizeInBytes = indexCount * detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);

    auto nativeBuffer = std::make_unique<BufferDirect3D11>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->initialize(
            device_.Get(),
            sizeInBytes,
            bufferUsage,
            toBindFlag(BufferBindMode::IndexBuffer));
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    if (auto err = checkError(infoQueue_.Get()); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "CheckError() failed"));
    }
#endif

    auto indexBuffer = std::make_shared<IndexBuffer>(
        std::move(nativeBuffer),
        elementSize,
        indexCount,
        bufferUsage);
    return std::make_tuple(std::move(indexBuffer), nullptr);
}

std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::createConstantBuffer(
    const void* sourceData,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    auto nativeBuffer = std::make_unique<BufferDirect3D11>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->initialize(
            device_.Get(),
            sourceData,
            sizeInBytes,
            bufferUsage,
            toBindFlag(BufferBindMode::ConstantBuffer));
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    if (auto err = checkError(infoQueue_.Get()); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "CheckError() failed"));
    }
#endif

    auto constantBuffer = std::make_shared<ConstantBuffer>(
        std::move(nativeBuffer),
        sizeInBytes,
        bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::createConstantBuffer(
    std::size_t sizeInBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(sizeInBytes > 0);

    auto nativeBuffer = std::make_unique<BufferDirect3D11>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->initialize(
            device_.Get(),
            sizeInBytes,
            bufferUsage,
            toBindFlag(BufferBindMode::ConstantBuffer));
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    if (auto err = checkError(infoQueue_.Get()); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "CheckError() failed"));
    }
#endif

    auto constantBuffer = std::make_shared<ConstantBuffer>(
        std::move(nativeBuffer),
        sizeInBytes,
        bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<PipelineState>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::createPipelineState(const PipelineDescriptor& descriptor) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    auto pipelineState = std::make_shared<PipelineStateDirect3D11>();

    if (auto err = pipelineState->initialize(device_.Get(), descriptor); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize PipelineStateDirect3D11"));
    }

    return std::make_tuple(std::move(pipelineState), nullptr);
}

std::tuple<std::shared_ptr<EffectReflection>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::createEffectReflection(
    const PipelineDescriptor& descriptor,
    [[maybe_unused]] const std::shared_ptr<PipelineState>& pipelineState) noexcept
{
    auto vertexShader = std::dynamic_pointer_cast<VertexShaderDirect3D11>(descriptor.vertexShader);
    if (vertexShader == nullptr) {
        return std::make_tuple(nullptr, errors::make("failed to cast VertexShader to VertexShaderDirect3D11"));
    }

    auto pixelShader = std::dynamic_pointer_cast<PixelShaderDirect3D11>(descriptor.pixelShader);
    if (pixelShader == nullptr) {
        return std::make_tuple(nullptr, errors::make("failed to cast PixelShader to PixelShaderDirect3D11"));
    }

    auto effectReflection = std::make_shared<EffectReflectionDirect3D11>();
    if (auto err = effectReflection->initialize(
            vertexShader->getShaderBytecode(),
            pixelShader->getShaderBytecode());
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize EffectReflectionDirect3D11"));
    }

    return std::make_tuple(std::move(effectReflection), nullptr);
}

std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::createShader(
    const detail::ShaderBytecode& shaderBytecode,
    const detail::ShaderCompileOptions& compileOptions) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);

    switch (compileOptions.profile.pipelineStage) {
    case ShaderPipelineStage::VertexShader: {
        auto shader = std::make_unique<VertexShaderDirect3D11>();
        if (auto err = shader->initialize(device_.Get(), shaderBytecode, compileOptions); err != nullptr) {
            return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize VertexShaderDirect3D11"));
        }
        return std::make_tuple(std::move(shader), nullptr);
    }
    case ShaderPipelineStage::PixelShader: {
        auto shader = std::make_unique<PixelShaderDirect3D11>();
        if (auto err = shader->initialize(device_.Get(), shaderBytecode, compileOptions); err != nullptr) {
            return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize PixelShaderDirect3D11"));
        }
        return std::make_tuple(std::move(shader), nullptr);
    }
    }
    return std::make_tuple(nullptr, errors::make("unsupported shader stage"));
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::createRenderTarget2D(
    std::int32_t width,
    std::int32_t height) noexcept
{
    return createRenderTarget2D(
        width,
        height,
        false,
        PixelFormat::R8G8B8A8_UNorm);
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::createRenderTarget2D(
    std::int32_t width,
    std::int32_t height,
    bool generateMipmap,
    PixelFormat format) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    const auto levelCount = generateMipmap
                                ? detail::TextureHelper::computeMipmapLevelCount(width, height)
                                : 1;

    // TODO: MSAA is not implemented yet.
    constexpr int multiSampleCount = 1;

    auto renderTarget = std::make_shared<RenderTarget2DDirect3D11>();

    if (auto err = renderTarget->initialize(
            device_.Get(),
            width,
            height,
            levelCount,
            format,
            multiSampleCount);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize RenderTarget2DDirect3D11"));
    }

    return std::make_tuple(std::move(renderTarget), nullptr);
}

std::tuple<std::shared_ptr<DepthStencilBuffer>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::createDepthStencilBuffer(
    std::int32_t width,
    std::int32_t height,
    PixelFormat depthStencilFormat) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    // TODO: MSAA is not implemented yet.
    constexpr int multiSampleCount = 1;

    auto depthStencilBuffer = std::make_shared<DepthStencilBufferDirect3D11>();

    if (auto err = depthStencilBuffer->initialize(
            device_.Get(),
            width,
            height,
            depthStencilFormat,
            multiSampleCount);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize DepthStencilBufferDirect3D11"));
    }

    return std::make_tuple(std::move(depthStencilBuffer), nullptr);
}

std::tuple<std::shared_ptr<SamplerState>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::createSamplerState(const SamplerDescriptor& descriptor) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    auto samplerState = std::make_shared<SamplerStateDirect3D11>();

    if (auto err = samplerState->initialize(device_.Get(), descriptor); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize SamplerStateDirect3D11"));
    }

    return std::make_tuple(std::move(samplerState), nullptr);
}

std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::createTexture2D(
    std::int32_t width,
    std::int32_t height) noexcept
{
    return createTexture2D(
        width,
        height,
        false,
        PixelFormat::R8G8B8A8_UNorm);
}

std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::createTexture2D(
    std::int32_t width,
    std::int32_t height,
    bool mipMap,
    PixelFormat format) noexcept
{
    POMDOG_ASSERT(device_ != nullptr);
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    const auto levelCount = mipMap
                                ? detail::TextureHelper::computeMipmapLevelCount(width, height)
                                : 1;

    auto texture = std::make_shared<Texture2DDirect3D11>();

    if (auto err = texture->initialize(
            device_.Get(),
            width,
            height,
            levelCount,
            format);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize Texture2DDirect3D11"));
    }

    return std::make_tuple(std::move(texture), nullptr);
}

Microsoft::WRL::ComPtr<ID3D11Device3>
GraphicsDeviceDirect3D11::getDevice() const noexcept
{
    return device_;
}

std::tuple<Microsoft::WRL::ComPtr<IDXGIFactory1>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::getDXGIFactory() noexcept
{
    return adapters_.getFactory();
}

void GraphicsDeviceDirect3D11::clientSizeChanged(int width, int height)
{
    presentationParameters_.backBufferWidth = width;
    presentationParameters_.backBufferHeight = height;
}

} // namespace pomdog::gpu::detail::direct3d11
