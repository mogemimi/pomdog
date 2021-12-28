// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/direct3d11/graphics_device_direct3d11.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/buffer_bind_mode.h"
#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/gpu/backends/graphics_command_list_immediate.h"
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
#include "pomdog/gpu/pipeline_state_description.h"
#include "pomdog/gpu/shader_language.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/logging/log.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::direct3d11 {
namespace {

using Microsoft::WRL::ComPtr;

std::tuple<ID3D11InfoQueue*, std::unique_ptr<Error>>
BuildInfoQueue(ID3D11Device* nativeDevice) noexcept
{
    POMDOG_ASSERT(nativeDevice != nullptr);

    ID3D11InfoQueue* infoQueue = nullptr;
    if (auto hr = nativeDevice->QueryInterface(IID_PPV_ARGS(&infoQueue)); FAILED(hr)) {
        if (hr == E_NOINTERFACE) {
            return std::make_tuple(nullptr, nullptr);
        }
        return std::make_tuple(nullptr, errors::New("failed to get ID3D11InfoQueue"));
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
        return std::make_tuple(nullptr, errors::New("AddStorageFilterEntries() failed"));
    }
    if (auto hr = infoQueue->AddRetrievalFilterEntries(&filter); FAILED(hr)) {
        return std::make_tuple(nullptr, errors::New("AddRetrievalFilterEntries() failed"));
    }

    return std::make_tuple(std::move(infoQueue), nullptr);
}

#if defined(DEBUG) && !defined(NDEBUG)
[[nodiscard]] std::unique_ptr<Error>
CheckError(ID3D11InfoQueue* infoQueue) noexcept
{
#if defined(DEBUG) && !defined(NDEBUG)
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
        return errors::New(std::move(message));
    }

    return nullptr;
}
#endif

[[nodiscard]] D3D11_BIND_FLAG
ToBindFlag(BufferBindMode bindMode) noexcept
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
BuildDevice(
    AdapterManager& adapters,
    Microsoft::WRL::ComPtr<ID3D11Device3>& device,
    D3D_DRIVER_TYPE& driverType,
    D3D_FEATURE_LEVEL& featureLevel) noexcept
{
    POMDOG_ASSERT(!device);

    UINT createDeviceFlags = 0;
#if defined(DEBUG) && !defined(NDEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    // NOTE: supported Direct2D1
    //createDeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    constexpr std::array<D3D_DRIVER_TYPE, 3> driverTypes = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    constexpr std::array<D3D_FEATURE_LEVEL, 7> featureLevels = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    HRESULT hr = S_OK;

    ComPtr<ID3D11Device> d3d11Device;

    auto adapter = adapters.ActiveAdapter();
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
        return errors::New("failed to create ID3D11Device");
    }

    // NOTE: Create ID3D11Device3 from ID3D11Device.
    if (hr = d3d11Device->QueryInterface(__uuidof(ID3D11Device3), &device); FAILED(hr)) {
        return errors::New("failed to create ID3D11Device3 from ID3D11Device");
    }
    d3d11Device.Reset();

#if defined(DEBUG) && !defined(NDEBUG)
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
            return errors::New("CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS) failed");
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
AdapterManager::EnumAdapters() noexcept
{
    Microsoft::WRL::ComPtr<IDXGIFactory1> dxgiFactory;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

    if (FAILED(hr)) {
        return errors::New("CreateDXGIFactory1() failed");
    }

    UINT i = 0;
    Microsoft::WRL::ComPtr<IDXGIAdapter1> newAdapter;
    while ((hr = dxgiFactory->EnumAdapters1(i, &newAdapter)) != DXGI_ERROR_NOT_FOUND) {
        if (FAILED(hr)) {
            return errors::New("failed to enumerate adapters");
        }

        adapters.push_back(newAdapter);
        newAdapter.Reset();
        ++i;
    }

    if (adapters.empty()) {
        return errors::New("List of DXGI adapters is empty");
    }

    POMDOG_ASSERT(!adapters.empty());
    activeAdapter = adapters.front();
    return nullptr;
}

void AdapterManager::Clear()
{
    activeAdapter.Reset();

    for (auto& adapter : adapters) {
        adapter.Reset();
    }
    adapters.clear();
}

IDXGIAdapter1* AdapterManager::ActiveAdapter() const
{
    POMDOG_ASSERT(activeAdapter);
    return activeAdapter.Get();
}

std::tuple<Microsoft::WRL::ComPtr<IDXGIFactory1>, std::unique_ptr<Error>>
AdapterManager::GetFactory() noexcept
{
    POMDOG_ASSERT(activeAdapter != nullptr);

    ComPtr<IDXGIFactory1> dxgiFactory;
    if (auto hr = activeAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)); FAILED(hr)) {
        return std::make_tuple(nullptr, errors::New("failed to get DXGIFactory1"));
    }

    return std::make_tuple(std::move(dxgiFactory), nullptr);
}

std::unique_ptr<Error>
GraphicsDeviceDirect3D11::Initialize(const PresentationParameters& presentationParametersIn) noexcept
{
    driverType = D3D_DRIVER_TYPE_NULL;
    featureLevel = D3D_FEATURE_LEVEL_11_1;
    presentationParameters = presentationParametersIn;

    if (auto err = adapters.EnumAdapters(); err != nullptr) {
        return errors::Wrap(std::move(err), "failed to enumerate adapters");
    }

    if (auto err = BuildDevice(adapters, device, driverType, featureLevel); err != nullptr) {
        return errors::Wrap(std::move(err), "BuildDevice() failed");
    }

    std::unique_ptr<Error> err;
    std::tie(infoQueue, err) = BuildInfoQueue(device.Get());
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "BuildInfoQueue() failed");
    }

    return nullptr;
}

GraphicsDeviceDirect3D11::~GraphicsDeviceDirect3D11() = default;

ShaderLanguage GraphicsDeviceDirect3D11::GetSupportedLanguage() const noexcept
{
    return ShaderLanguage::HLSL;
}

PresentationParameters GraphicsDeviceDirect3D11::GetPresentationParameters() const noexcept
{
    return presentationParameters;
}

std::tuple<std::shared_ptr<GraphicsCommandList>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::CreateGraphicsCommandList() noexcept
{
    auto commandList = std::make_shared<GraphicsCommandListImmediate>();
    return std::make_tuple(std::move(commandList), nullptr);
}

std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::CreateVertexBuffer(
    const void* vertices,
    std::size_t vertexCount,
    std::size_t strideBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(vertices != nullptr);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(strideBytes > 0);

    const auto sizeInBytes = vertexCount * strideBytes;

    auto nativeBuffer = std::make_unique<BufferDirect3D11>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->Initialize(
            device.Get(),
            vertices,
            sizeInBytes,
            bufferUsage,
            ToBindFlag(BufferBindMode::VertexBuffer));
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(DEBUG) && !defined(NDEBUG)
    if (auto err = CheckError(infoQueue.Get()); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "CheckError() failed"));
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
GraphicsDeviceDirect3D11::CreateVertexBuffer(
    std::size_t vertexCount,
    std::size_t strideBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(strideBytes > 0);

    const auto sizeInBytes = vertexCount * strideBytes;

    auto nativeBuffer = std::make_unique<BufferDirect3D11>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->Initialize(
            device.Get(),
            sizeInBytes,
            bufferUsage,
            ToBindFlag(BufferBindMode::VertexBuffer));
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(DEBUG) && !defined(NDEBUG)
    if (auto err = CheckError(infoQueue.Get()); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "CheckError() failed"));
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
GraphicsDeviceDirect3D11::CreateIndexBuffer(
    IndexElementSize elementSize,
    const void* indices,
    std::size_t indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(indexCount > 0);

    const auto sizeInBytes = indexCount * detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);

    auto nativeBuffer = std::make_unique<BufferDirect3D11>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->Initialize(
            device.Get(),
            indices,
            sizeInBytes,
            bufferUsage,
            ToBindFlag(BufferBindMode::IndexBuffer));
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(DEBUG) && !defined(NDEBUG)
    if (auto err = CheckError(infoQueue.Get()); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "CheckError() failed"));
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
GraphicsDeviceDirect3D11::CreateIndexBuffer(
    IndexElementSize elementSize,
    std::size_t indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(indexCount > 0);

    const auto sizeInBytes = indexCount * detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);

    auto nativeBuffer = std::make_unique<BufferDirect3D11>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->Initialize(
            device.Get(),
            sizeInBytes,
            bufferUsage,
            ToBindFlag(BufferBindMode::IndexBuffer));
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(DEBUG) && !defined(NDEBUG)
    if (auto err = CheckError(infoQueue.Get()); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "CheckError() failed"));
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
GraphicsDeviceDirect3D11::CreateConstantBuffer(
    const void* sourceData,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    auto nativeBuffer = std::make_unique<BufferDirect3D11>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->Initialize(
            device.Get(),
            sourceData,
            sizeInBytes,
            bufferUsage,
            ToBindFlag(BufferBindMode::ConstantBuffer));
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(DEBUG) && !defined(NDEBUG)
    if (auto err = CheckError(infoQueue.Get()); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "CheckError() failed"));
    }
#endif

    auto constantBuffer = std::make_shared<ConstantBuffer>(
        std::move(nativeBuffer),
        sizeInBytes,
        bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::CreateConstantBuffer(
    std::size_t sizeInBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(sizeInBytes > 0);

    auto nativeBuffer = std::make_unique<BufferDirect3D11>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->Initialize(
            device.Get(),
            sizeInBytes,
            bufferUsage,
            ToBindFlag(BufferBindMode::ConstantBuffer));
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(DEBUG) && !defined(NDEBUG)
    if (auto err = CheckError(infoQueue.Get()); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "CheckError() failed"));
    }
#endif

    auto constantBuffer = std::make_shared<ConstantBuffer>(
        std::move(nativeBuffer),
        sizeInBytes,
        bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<PipelineState>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::CreatePipelineState(const PipelineStateDescription& description) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    auto pipelineState = std::make_shared<PipelineStateDirect3D11>();

    if (auto err = pipelineState->Initialize(device.Get(), description); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize PipelineStateDirect3D11"));
    }

    return std::make_tuple(std::move(pipelineState), nullptr);
}

std::tuple<std::shared_ptr<EffectReflection>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::CreateEffectReflection(
    const PipelineStateDescription& description,
    [[maybe_unused]] const std::shared_ptr<PipelineState>& pipelineState) noexcept
{
    auto vertexShader = std::dynamic_pointer_cast<VertexShaderDirect3D11>(description.VertexShader);
    if (vertexShader == nullptr) {
        return std::make_tuple(nullptr, errors::New("failed to cast VertexShader to VertexShaderDirect3D11"));
    }

    auto pixelShader = std::dynamic_pointer_cast<PixelShaderDirect3D11>(description.PixelShader);
    if (pixelShader == nullptr) {
        return std::make_tuple(nullptr, errors::New("failed to cast PixelShader to PixelShaderDirect3D11"));
    }

    auto effectReflection = std::make_shared<EffectReflectionDirect3D11>();
    if (auto err = effectReflection->Initialize(
            vertexShader->GetShaderBytecode(),
            pixelShader->GetShaderBytecode());
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize EffectReflectionDirect3D11"));
    }

    return std::make_tuple(std::move(effectReflection), nullptr);
}

std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::CreateShader(
    const detail::ShaderBytecode& shaderBytecode,
    const detail::ShaderCompileOptions& compileOptions) noexcept
{
    POMDOG_ASSERT(device != nullptr);

    switch (compileOptions.Profile.PipelineStage) {
    case ShaderPipelineStage::VertexShader: {
        auto shader = std::make_unique<VertexShaderDirect3D11>();
        if (auto err = shader->Initialize(device.Get(), shaderBytecode, compileOptions); err != nullptr) {
            return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize VertexShaderDirect3D11"));
        }
        return std::make_tuple(std::move(shader), nullptr);
    }
    case ShaderPipelineStage::PixelShader: {
        auto shader = std::make_unique<PixelShaderDirect3D11>();
        if (auto err = shader->Initialize(device.Get(), shaderBytecode, compileOptions); err != nullptr) {
            return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize PixelShaderDirect3D11"));
        }
        return std::make_tuple(std::move(shader), nullptr);
    }
    }
    return std::make_tuple(nullptr, errors::New("unsupported shader stage"));
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::CreateRenderTarget2D(
    std::int32_t width,
    std::int32_t height) noexcept
{
    return CreateRenderTarget2D(
        width,
        height,
        false,
        SurfaceFormat::R8G8B8A8_UNorm);
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::CreateRenderTarget2D(
    std::int32_t width,
    std::int32_t height,
    bool generateMipmap,
    SurfaceFormat format) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    const auto levelCount = generateMipmap
        ? detail::TextureHelper::ComputeMipmapLevelCount(width, height)
        : 1;

    // TODO: MSAA is not implemented yet.
    constexpr int multiSampleCount = 1;

    auto renderTarget = std::make_shared<RenderTarget2DDirect3D11>();

    if (auto err = renderTarget->Initialize(
            device.Get(),
            width,
            height,
            levelCount,
            format,
            multiSampleCount);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize RenderTarget2DDirect3D11"));
    }

    return std::make_tuple(std::move(renderTarget), nullptr);
}

std::tuple<std::shared_ptr<DepthStencilBuffer>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::CreateDepthStencilBuffer(
    std::int32_t width,
    std::int32_t height,
    SurfaceFormat depthStencilFormat) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    // TODO: MSAA is not implemented yet.
    constexpr int multiSampleCount = 1;

    auto depthStencilBuffer = std::make_shared<DepthStencilBufferDirect3D11>();

    if (auto err = depthStencilBuffer->Initialize(
            device.Get(),
            width,
            height,
            depthStencilFormat,
            multiSampleCount);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize DepthStencilBufferDirect3D11"));
    }

    return std::make_tuple(std::move(depthStencilBuffer), nullptr);
}

std::tuple<std::shared_ptr<SamplerState>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::CreateSamplerState(const SamplerDescription& description) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    auto samplerState = std::make_shared<SamplerStateDirect3D11>();

    if (auto err = samplerState->Initialize(device.Get(), description); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize SamplerStateDirect3D11"));
    }

    return std::make_tuple(std::move(samplerState), nullptr);
}

std::tuple<std::shared_ptr<Texture2D>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::CreateTexture2D(
    std::int32_t width,
    std::int32_t height) noexcept
{
    return CreateTexture2D(
        width,
        height,
        false,
        SurfaceFormat::R8G8B8A8_UNorm);
}

std::tuple<std::shared_ptr<Texture2D>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::CreateTexture2D(
    std::int32_t width,
    std::int32_t height,
    bool mipMap,
    SurfaceFormat format) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    const auto levelCount = mipMap
        ? detail::TextureHelper::ComputeMipmapLevelCount(width, height)
        : 1;

    auto texture = std::make_shared<Texture2DDirect3D11>();

    if (auto err = texture->Initialize(
            device.Get(),
            width,
            height,
            levelCount,
            format);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize Texture2DDirect3D11"));
    }

    return std::make_tuple(std::move(texture), nullptr);
}

Microsoft::WRL::ComPtr<ID3D11Device3>
GraphicsDeviceDirect3D11::GetDevice() const noexcept
{
    return device;
}

std::tuple<Microsoft::WRL::ComPtr<IDXGIFactory1>, std::unique_ptr<Error>>
GraphicsDeviceDirect3D11::GetDXGIFactory() noexcept
{
    return adapters.GetFactory();
}

void GraphicsDeviceDirect3D11::ClientSizeChanged(int width, int height)
{
    presentationParameters.BackBufferWidth = width;
    presentationParameters.BackBufferHeight = height;
}

} // namespace pomdog::detail::direct3d11
