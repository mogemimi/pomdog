// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "GraphicsDeviceDirect3D11.hpp"
#include "BufferDirect3D11.hpp"
#include "EffectReflectionDirect3D11.hpp"
#include "PipelineStateDirect3D11.hpp"
#include "RenderTarget2DDirect3D11.hpp"
#include "SamplerStateDirect3D11.hpp"
#include "Texture2DDirect3D11.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "../RenderSystem/ShaderCompileOptions.hpp"
#include "Pomdog/Graphics/BufferUsage.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/StringFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <array>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {
namespace {

using Microsoft::WRL::ComPtr;

class AdapterManager final {
private:
    std::vector<ComPtr<IDXGIAdapter1>> adapters;
    ComPtr<IDXGIAdapter1> activeAdapter;

public:
    void EnumAdapters();
    void Clear();

    IDXGIAdapter1* ActiveAdapter() const
    {
        POMDOG_ASSERT(activeAdapter);
        return activeAdapter.Get();
    }

    ComPtr<IDXGIFactory1> GetFactory()
    {
        ComPtr<IDXGIFactory1> dxgiFactory;

        POMDOG_ASSERT(activeAdapter);
        HRESULT hr = activeAdapter->GetParent(__uuidof(IDXGIFactory1), &dxgiFactory);
        if (FAILED(hr))
        {
            ///@todo throw exception

            // FUS RO DAH!
            Log::Internal("Failed to get DXGIFactory1");
        }

        return std::move(dxgiFactory);
    }
};
//-----------------------------------------------------------------------
void AdapterManager::EnumAdapters()
{
    Microsoft::WRL::ComPtr<IDXGIFactory1> dxgiFactory;
    HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(dxgiFactory.GetAddressOf()));

    if (FAILED(hr))
    {
        // error
        ///@todo throw exception
        Log::Internal("Failed to create DXGIFactory1.");
        return;
    }

    UINT i = 0;
    Microsoft::WRL::ComPtr<IDXGIAdapter1> newAdapter;
    while ((hr = dxgiFactory->EnumAdapters1(i, &newAdapter)) != DXGI_ERROR_NOT_FOUND)
    {
        if (FAILED(hr))
        {
            // error
            ///@todo throw exception
            Log::Internal("Failed to enumerate adapters.");
            break;
        }

        adapters.push_back(newAdapter);
        newAdapter.Reset();
        ++i;
    }

    if (adapters.empty())
    {
        // error
        ///@todo throw exception
        return;
    }

    POMDOG_ASSERT(!adapters.empty());
    activeAdapter = adapters.front();
}
//-----------------------------------------------------------------------
void AdapterManager::Clear()
{
    activeAdapter.Reset();

    for (auto & adapter : adapters) {
        adapter.Reset();
    }
    adapters.clear();
}
//-----------------------------------------------------------------------
static ID3D11InfoQueue* BuildInfoQueue(ID3D11Device* nativeDevice)
{
    POMDOG_ASSERT(nativeDevice);

    ID3D11InfoQueue* infoQueue = nullptr;
    HRESULT hr = nativeDevice->QueryInterface(__uuidof(ID3D11InfoQueue), reinterpret_cast<void**>(&infoQueue));

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
    filter.DenyList.NumSeverities = severityList.size();

    infoQueue->AddStorageFilterEntries(&filter);
    infoQueue->AddRetrievalFilterEntries(&filter);

    return std::move(infoQueue);
}
//-----------------------------------------------------------------------
static void CheckError(ID3D11InfoQueue* infoQueue)
{
#if defined(DEBUG) && !defined(NDEBUG)
    POMDOG_ASSERT(infoQueue);
#endif

    if (!infoQueue) {
        return;
    }

    const auto storedMessageCount = infoQueue->GetNumStoredMessages();

    std::string message;
    for (UINT64 i = 0; i < storedMessageCount; ++i)
    {
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

    if (message.empty())
    {
        Log::Internal(message);
    }
}
//-----------------------------------------------------------------------
static D3D11_BIND_FLAG ToBindFlag(BufferBindMode bindMode) noexcept
{
    switch (bindMode) {
    case BufferBindMode::ConstantBuffer: return D3D11_BIND_CONSTANT_BUFFER;
    case BufferBindMode::IndexBuffer: return D3D11_BIND_INDEX_BUFFER;
    case BufferBindMode::VertexBuffer:
        break;
    }
    return D3D11_BIND_VERTEX_BUFFER;
}

} // unnamed namespace
//-----------------------------------------------------------------------
class GraphicsDeviceDirect3D11::Impl {
public:
    AdapterManager adapters;
    ComPtr<ID3D11Device> nativeDevice;
    ComPtr<ID3D11DeviceContext> deviceContext;
    ComPtr<ID3D11InfoQueue> infoQueue;
    D3D_DRIVER_TYPE driverType;
    D3D_FEATURE_LEVEL featureLevel;

public:
    Impl();

private:
    void BuildDevice();
};
//-----------------------------------------------------------------------
GraphicsDeviceDirect3D11::Impl::Impl()
    : driverType(D3D_DRIVER_TYPE_NULL)
    , featureLevel(D3D_FEATURE_LEVEL_11_0)
{
    adapters.EnumAdapters();
    BuildDevice();

    infoQueue = BuildInfoQueue(nativeDevice.Get());
}
//-----------------------------------------------------------------------
void GraphicsDeviceDirect3D11::Impl::BuildDevice()
{
    POMDOG_ASSERT(!nativeDevice);
    POMDOG_ASSERT(!deviceContext);

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

    constexpr std::array<D3D_FEATURE_LEVEL, 6> featureLevels = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    HRESULT hr = S_OK;

    auto adapter = adapters.ActiveAdapter();
    for (auto & type: driverTypes)
    {
        driverType = type;
        hr = D3D11CreateDevice(adapter
            , (nullptr != adapter ? D3D_DRIVER_TYPE_UNKNOWN : driverType)
            , nullptr
            , createDeviceFlags
            , featureLevels.data()
            , static_cast<UINT>(featureLevels.size())
            , D3D11_SDK_VERSION
            , &nativeDevice
            , &featureLevel
            , &deviceContext);

        if (SUCCEEDED(hr)) {
            break;
        }
    }

    if (FAILED(hr))
    {
        ///@todo throw exception

        // FUS RO DAH!
        Log::Internal("Failed to create ID3D11Device");
        return;
    }

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
        };
    }
#endif
}
//-----------------------------------------------------------------------
GraphicsDeviceDirect3D11::GraphicsDeviceDirect3D11()
    : impl(std::make_unique<Impl>())
{}
//-----------------------------------------------------------------------
GraphicsDeviceDirect3D11::~GraphicsDeviceDirect3D11() = default;
//-----------------------------------------------------------------------
ShaderLanguage GraphicsDeviceDirect3D11::GetSupportedLanguage() const
{
    return ShaderLanguage::HLSL;
}
//-----------------------------------------------------------------------
std::unique_ptr<Shader>
GraphicsDeviceDirect3D11::CreateShader(ShaderBytecode const& shaderBytecode,
    ShaderCompileOptions const& compileOptions)
{
    switch (compileOptions.Profile.PipelineStage) {
    case ShaderPipelineStage::VertexShader: {
        return std::make_unique<VertexShaderDirect3D11>(
            impl->nativeDevice.Get(), shaderBytecode, compileOptions);
    }
    case ShaderPipelineStage::PixelShader: {
        return std::make_unique<PixelShaderDirect3D11>(
            impl->nativeDevice.Get(), shaderBytecode, compileOptions);
    }
    }

    POMDOG_THROW_EXCEPTION(std::domain_error, "Failed to create shader");
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeBuffer>
GraphicsDeviceDirect3D11::CreateBuffer(std::size_t sizeInBytes,
    BufferUsage bufferUsage, BufferBindMode bindMode)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeDevice);
    POMDOG_ASSERT(impl->deviceContext);

    try {
        return std::make_unique<BufferDirect3D11>(
            impl->nativeDevice.Get(),
            impl->deviceContext.Get(),
            sizeInBytes,
            bufferUsage,
            ToBindFlag(bindMode));
    }
    catch (std::exception const& e) {
#if defined(DEBUG) && !defined(NDEBUG)
        CheckError(impl->infoQueue.Get());
#endif
        throw e;
    }
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeBuffer>
GraphicsDeviceDirect3D11::CreateBuffer(
    void const* sourceData, std::size_t sizeInBytes,
    BufferUsage bufferUsage, BufferBindMode bindMode)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeDevice);
    POMDOG_ASSERT(impl->deviceContext);

    try {
        return std::make_unique<BufferDirect3D11>(
            impl->nativeDevice.Get(),
            impl->deviceContext.Get(),
            sourceData,
            sizeInBytes,
            bufferUsage,
            ToBindFlag(bindMode));
    }
    catch (std::exception const& e) {
#if defined(DEBUG) && !defined(NDEBUG)
        CheckError(impl->infoQueue.Get());
#endif
        throw e;
    }
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeSamplerState>
GraphicsDeviceDirect3D11::CreateSamplerState(SamplerDescription const& description)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeDevice);

    return std::make_unique<SamplerStateDirect3D11>(impl->nativeDevice.Get(), description);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativePipelineState>
GraphicsDeviceDirect3D11::CreatePipelineState(PipelineStateDescription const& description)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeDevice);

    return std::make_unique<PipelineStateDirect3D11>(impl->nativeDevice.Get(), description);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeEffectReflection>
GraphicsDeviceDirect3D11::CreateEffectReflection(NativePipelineState & pipelineStateIn)
{
    auto const pipelineState = dynamic_cast<PipelineStateDirect3D11*>(&pipelineStateIn);
    POMDOG_ASSERT(pipelineState != nullptr);

    if (pipelineState == nullptr) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::domain_error,
            "Failed to cast pipeline state to PipelineStateDirect3D11");
    }

    return std::make_unique<EffectReflectionDirect3D11>(
        pipelineState->GetVertexShaderBlob(),
        pipelineState->GetPixelShaderBlob());
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeTexture2D>
GraphicsDeviceDirect3D11::CreateTexture2D(std::int32_t width, std::int32_t height, std::uint32_t mipmapLevels,
    SurfaceFormat format)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeDevice);
    POMDOG_ASSERT(impl->deviceContext);

    return std::make_unique<Texture2DDirect3D11>(impl->nativeDevice.Get(),
        impl->deviceContext, width, height, mipmapLevels, format);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeRenderTarget2D>
GraphicsDeviceDirect3D11::CreateRenderTarget2D(std::int32_t width, std::int32_t height,
    std::uint32_t mipmapLevels, SurfaceFormat format, DepthFormat depthStencilFormat)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->nativeDevice);
    return std::make_unique<RenderTarget2DDirect3D11>(impl->nativeDevice.Get(),
        width, height, mipmapLevels, format, depthStencilFormat);
}
//-----------------------------------------------------------------------
Microsoft::WRL::ComPtr<ID3D11DeviceContext> GraphicsDeviceDirect3D11::DeviceContext() const
{
    POMDOG_ASSERT(impl);
    return impl->deviceContext;
}
//-----------------------------------------------------------------------
Microsoft::WRL::ComPtr<ID3D11Device> GraphicsDeviceDirect3D11::NativeDevice() const
{
    POMDOG_ASSERT(impl);
    return impl->nativeDevice;
}
//-----------------------------------------------------------------------
Microsoft::WRL::ComPtr<IDXGIFactory1> GraphicsDeviceDirect3D11::DXGIFactory() const
{
    POMDOG_ASSERT(impl);
    return impl->adapters.GetFactory();
}
//-----------------------------------------------------------------------
} // namespace Direct3D11
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog
