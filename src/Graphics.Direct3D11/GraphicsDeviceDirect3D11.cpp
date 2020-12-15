// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "GraphicsDeviceDirect3D11.hpp"
#include "BufferDirect3D11.hpp"
#include "EffectReflectionDirect3D11.hpp"
#include "PipelineStateDirect3D11.hpp"
#include "RenderTarget2DDirect3D11.hpp"
#include "SamplerStateDirect3D11.hpp"
#include "ShaderDirect3D11.hpp"
#include "Texture2DDirect3D11.hpp"
#include "../Graphics.Backends/BufferBindMode.hpp"
#include "../Graphics.Backends/BufferHelper.hpp"
#include "../Graphics.Backends/GraphicsCommandListImmediate.hpp"
#include "../Graphics.Backends/ShaderBytecode.hpp"
#include "../Graphics.Backends/ShaderCompileOptions.hpp"
#include "../Graphics.Backends/TextureHelper.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/PipelineStateDescription.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <array>

namespace Pomdog::Detail::Direct3D11 {
namespace {

using Microsoft::WRL::ComPtr;

ID3D11InfoQueue* BuildInfoQueue(ID3D11Device* nativeDevice)
{
    POMDOG_ASSERT(nativeDevice);

    ID3D11InfoQueue* infoQueue = nullptr;
    HRESULT hr = nativeDevice->QueryInterface(IID_PPV_ARGS(&infoQueue));

    POMDOG_ASSERT(SUCCEEDED(hr));

    if (FAILED(hr)) {
        // error
        ///@todo throw exception
    }

    if (!infoQueue) {
        return nullptr;
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

    infoQueue->AddStorageFilterEntries(&filter);
    infoQueue->AddRetrievalFilterEntries(&filter);

    return std::move(infoQueue);
}

void CheckError(ID3D11InfoQueue* infoQueue)
{
#if defined(DEBUG) && !defined(NDEBUG)
    POMDOG_ASSERT(infoQueue);
#endif

    if (!infoQueue) {
        return;
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

    if (message.empty()) {
        Log::Internal(message);
    }
}

D3D11_BIND_FLAG ToBindFlag(BufferBindMode bindMode) noexcept
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

[[nodiscard]] std::shared_ptr<Error>
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
        return Errors::New("failed to create ID3D11Device");
    }

    // NOTE: Create ID3D11Device3 from ID3D11Device.
    if (hr = d3d11Device->QueryInterface(__uuidof(ID3D11Device3), &device); FAILED(hr)) {
        return Errors::New("failed to create ID3D11Device3 from ID3D11Device");
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
        }
    }
    {
        D3D11_FEATURE_DATA_D3D11_OPTIONS d3d11Options;
        hr = device->CheckFeatureSupport(
            D3D11_FEATURE_D3D11_OPTIONS,
            &d3d11Options,
            sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS));

        if (FAILED(hr)) {
            return Errors::New("CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS) failed");
        }

        Log::Internal("MapNoOverwriteOnDynamicBufferSRV: "
            + std::to_string(d3d11Options.MapNoOverwriteOnDynamicBufferSRV));
        Log::Internal("MapNoOverwriteOnDynamicConstantBuffer: "
            + std::to_string(d3d11Options.MapNoOverwriteOnDynamicConstantBuffer));
    }
#endif
    return nullptr;
}

} // namespace

std::shared_ptr<Error>
AdapterManager::EnumAdapters() noexcept
{
    Microsoft::WRL::ComPtr<IDXGIFactory1> dxgiFactory;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

    if (FAILED(hr)) {
        return Errors::New("CreateDXGIFactory1() failed");
    }

    UINT i = 0;
    Microsoft::WRL::ComPtr<IDXGIAdapter1> newAdapter;
    while ((hr = dxgiFactory->EnumAdapters1(i, &newAdapter)) != DXGI_ERROR_NOT_FOUND) {
        if (FAILED(hr)) {
            return Errors::New("failed to enumerate adapters");
        }

        adapters.push_back(newAdapter);
        newAdapter.Reset();
        ++i;
    }

    if (adapters.empty()) {
        return Errors::New("List of DXGI adapters is empty");
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

std::tuple<Microsoft::WRL::ComPtr<IDXGIFactory1>, std::shared_ptr<Error>>
AdapterManager::GetFactory() noexcept
{
    ComPtr<IDXGIFactory1> dxgiFactory;

    POMDOG_ASSERT(activeAdapter);
    HRESULT hr = activeAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
    if (FAILED(hr)) {
        return std::make_tuple(nullptr, Errors::New("failed to get DXGIFactory1"));
    }

    return std::make_tuple(std::move(dxgiFactory), nullptr);
}

std::shared_ptr<Error>
GraphicsDeviceDirect3D11::Initialize(const PresentationParameters& presentationParametersIn) noexcept
{
    driverType = D3D_DRIVER_TYPE_NULL;
    featureLevel = D3D_FEATURE_LEVEL_11_1;
    presentationParameters = presentationParametersIn;

    if (auto err = adapters.EnumAdapters(); err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to enumerate adapters");
    }

    if (auto err = BuildDevice(adapters, device, driverType, featureLevel); err != nullptr) {
        return Errors::Wrap(std::move(err), "BuildDevice() failed");
    }

    infoQueue = BuildInfoQueue(device.Get());

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

std::tuple<std::shared_ptr<GraphicsCommandList>, std::shared_ptr<Error>>
GraphicsDeviceDirect3D11::CreateGraphicsCommandList() noexcept
{
    auto commandList = std::make_shared<GraphicsCommandListImmediate>();
    return std::make_tuple(std::move(commandList), nullptr);
}

std::tuple<std::shared_ptr<VertexBuffer>, std::shared_ptr<Error>>
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
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(DEBUG) && !defined(NDEBUG)
    CheckError(infoQueue.Get());
#endif

    auto vertexBuffer = std::make_shared<VertexBuffer>(
        std::move(nativeBuffer),
        vertexCount,
        strideBytes,
        bufferUsage);
    return std::make_tuple(std::move(vertexBuffer), nullptr);
}

std::tuple<std::shared_ptr<VertexBuffer>, std::shared_ptr<Error>>
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
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(DEBUG) && !defined(NDEBUG)
    CheckError(infoQueue.Get());
#endif

    auto vertexBuffer = std::make_shared<VertexBuffer>(
        std::move(nativeBuffer),
        vertexCount,
        strideBytes,
        bufferUsage);
    return std::make_tuple(std::move(vertexBuffer), nullptr);
}

std::tuple<std::shared_ptr<IndexBuffer>, std::shared_ptr<Error>>
GraphicsDeviceDirect3D11::CreateIndexBuffer(
    IndexElementSize elementSize,
    const void* indices,
    std::size_t indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(indexCount > 0);

    const auto sizeInBytes = indexCount * Detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);

    auto nativeBuffer = std::make_unique<BufferDirect3D11>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->Initialize(
            device.Get(),
            indices,
            sizeInBytes,
            bufferUsage,
            ToBindFlag(BufferBindMode::IndexBuffer));
        err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(DEBUG) && !defined(NDEBUG)
    CheckError(infoQueue.Get());
#endif

    auto indexBuffer = std::make_shared<IndexBuffer>(
        std::move(nativeBuffer),
        elementSize,
        indexCount,
        bufferUsage);
    return std::make_tuple(std::move(indexBuffer), nullptr);
}

std::tuple<std::shared_ptr<IndexBuffer>, std::shared_ptr<Error>>
GraphicsDeviceDirect3D11::CreateIndexBuffer(
    IndexElementSize elementSize,
    std::size_t indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(indexCount > 0);

    const auto sizeInBytes = indexCount * Detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);

    auto nativeBuffer = std::make_unique<BufferDirect3D11>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->Initialize(
            device.Get(),
            sizeInBytes,
            bufferUsage,
            ToBindFlag(BufferBindMode::IndexBuffer));
        err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(DEBUG) && !defined(NDEBUG)
    CheckError(infoQueue.Get());
#endif

    auto indexBuffer = std::make_shared<IndexBuffer>(
        std::move(nativeBuffer),
        elementSize,
        indexCount,
        bufferUsage);
    return std::make_tuple(std::move(indexBuffer), nullptr);
}

std::tuple<std::shared_ptr<ConstantBuffer>, std::shared_ptr<Error>>
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
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(DEBUG) && !defined(NDEBUG)
    CheckError(infoQueue.Get());
#endif

    auto constantBuffer = std::make_shared<ConstantBuffer>(
        std::move(nativeBuffer),
        sizeInBytes,
        bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<ConstantBuffer>, std::shared_ptr<Error>>
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
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize BufferDirect3D11"));
    }

#if defined(DEBUG) && !defined(NDEBUG)
    CheckError(infoQueue.Get());
#endif

    auto constantBuffer = std::make_shared<ConstantBuffer>(
        std::move(nativeBuffer),
        sizeInBytes,
        bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<PipelineState>, std::shared_ptr<Error>>
GraphicsDeviceDirect3D11::CreatePipelineState(const PipelineStateDescription& description) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    auto pipelineState = std::make_shared<PipelineStateDirect3D11>();

    if (auto err = pipelineState->Initialize(device.Get(), description); err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize PipelineStateDirect3D11"));
    }

    return std::make_tuple(std::move(pipelineState), nullptr);
}

std::tuple<std::shared_ptr<EffectReflection>, std::shared_ptr<Error>>
GraphicsDeviceDirect3D11::CreateEffectReflection(
    const PipelineStateDescription& description,
    [[maybe_unused]] const std::shared_ptr<PipelineState>& pipelineState) noexcept
{
    auto vertexShader = std::dynamic_pointer_cast<VertexShaderDirect3D11>(description.VertexShader);
    if (vertexShader == nullptr) {
        return std::make_tuple(nullptr, Errors::New("failed to cast VertexShader to VertexShaderDirect3D11"));
    }

    auto pixelShader = std::dynamic_pointer_cast<PixelShaderDirect3D11>(description.PixelShader);
    if (pixelShader == nullptr) {
        return std::make_tuple(nullptr, Errors::New("failed to cast PixelShader to PixelShaderDirect3D11"));
    }

    auto effectReflection = std::make_shared<EffectReflectionDirect3D11>();
    if (auto err = effectReflection->Initialize(
            vertexShader->GetShaderBytecode(),
            pixelShader->GetShaderBytecode());
        err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize EffectReflectionDirect3D11"));
    }

    return std::make_tuple(std::move(effectReflection), nullptr);
}

std::tuple<std::unique_ptr<Shader>, std::shared_ptr<Error>>
GraphicsDeviceDirect3D11::CreateShader(
    const Detail::ShaderBytecode& shaderBytecode,
    const Detail::ShaderCompileOptions& compileOptions) noexcept
{
    POMDOG_ASSERT(device != nullptr);

    switch (compileOptions.Profile.PipelineStage) {
    case ShaderPipelineStage::VertexShader: {
        auto shader = std::make_unique<VertexShaderDirect3D11>();
        if (auto err = shader->Initialize(device.Get(), shaderBytecode, compileOptions); err != nullptr) {
            return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize VertexShaderDirect3D11"));
        }
        return std::make_tuple(std::move(shader), nullptr);
    }
    case ShaderPipelineStage::PixelShader: {
        auto shader = std::make_unique<PixelShaderDirect3D11>();
        if (auto err = shader->Initialize(device.Get(), shaderBytecode, compileOptions); err != nullptr) {
            return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize PixelShaderDirect3D11"));
        }
        return std::make_tuple(std::move(shader), nullptr);
    }
    }
    return std::make_tuple(nullptr, Errors::New("unsupported shader stage"));
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::shared_ptr<Error>>
GraphicsDeviceDirect3D11::CreateRenderTarget2D(
    std::int32_t width,
    std::int32_t height) noexcept
{
    return CreateRenderTarget2D(
        width,
        height,
        false,
        SurfaceFormat::R8G8B8A8_UNorm,
        SurfaceFormat::Invalid);
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::shared_ptr<Error>>
GraphicsDeviceDirect3D11::CreateRenderTarget2D(
    std::int32_t width,
    std::int32_t height,
    bool generateMipmap,
    SurfaceFormat format,
    SurfaceFormat depthStencilFormat) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    const auto levelCount = generateMipmap
        ? Detail::TextureHelper::ComputeMipmapLevelCount(width, height)
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
            depthStencilFormat,
            multiSampleCount);
        err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize RenderTarget2DDirect3D11"));
    }

    return std::make_tuple(std::move(renderTarget), nullptr);
}

std::tuple<std::shared_ptr<SamplerState>, std::shared_ptr<Error>>
GraphicsDeviceDirect3D11::CreateSamplerState(const SamplerDescription& description) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    auto samplerState = std::make_shared<SamplerStateDirect3D11>();

    if (auto err = samplerState->Initialize(device.Get(), description); err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize SamplerStateDirect3D11"));
    }

    return std::make_tuple(std::move(samplerState), nullptr);
}

std::tuple<std::shared_ptr<Texture2D>, std::shared_ptr<Error>>
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

std::tuple<std::shared_ptr<Texture2D>, std::shared_ptr<Error>>
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
        ? Detail::TextureHelper::ComputeMipmapLevelCount(width, height)
        : 1;

    auto texture = std::make_shared<Texture2DDirect3D11>();

    if (auto err = texture->Initialize(
            device.Get(),
            width,
            height,
            levelCount,
            format);
        err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize Texture2DDirect3D11"));
    }

    return std::make_tuple(std::move(texture), nullptr);
}

Microsoft::WRL::ComPtr<ID3D11Device3>
GraphicsDeviceDirect3D11::GetDevice() const noexcept
{
    return device;
}

std::tuple<Microsoft::WRL::ComPtr<IDXGIFactory1>, std::shared_ptr<Error>>
GraphicsDeviceDirect3D11::GetDXGIFactory() noexcept
{
    return adapters.GetFactory();
}

void GraphicsDeviceDirect3D11::ClientSizeChanged(int width, int height)
{
    presentationParameters.BackBufferWidth = width;
    presentationParameters.BackBufferHeight = height;
}

} // namespace Pomdog::Detail::Direct3D11
