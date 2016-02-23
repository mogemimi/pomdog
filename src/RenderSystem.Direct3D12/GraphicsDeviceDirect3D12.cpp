// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "GraphicsDeviceDirect3D12.hpp"
#include "BufferDirect3D12.hpp"
#include "EffectReflectionDirect3D12.hpp"
#include "PipelineStateDirect3D12.hpp"
#include "RenderTarget2DDirect3D12.hpp"
#include "SamplerStateDirect3D12.hpp"
#include "ShaderDirect3D12.hpp"
#include "Texture2DDirect3D12.hpp"
#include "../RenderSystem.Direct3D/HLSLCompiling.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "../RenderSystem/ShaderCompileOptions.hpp"
#include "Pomdog/Graphics/PipelineStateDescription.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Logging/Log.hpp"
#include <dxgi1_4.h>

namespace Pomdog {
namespace Detail {
namespace Direct3D12 {
namespace {

using Microsoft::WRL::ComPtr;

void EnumerateAdapters(std::vector<ComPtr<IDXGIAdapter1>> & adapters)
{
    // Create DXGI factory
    ComPtr<IDXGIFactory3> dxgiFactory;
    HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create IDXGIFactory3");
    }

    // Enumerate adapters
    UINT index = 0;
    ComPtr<IDXGIAdapter1> newAdapter;
    while ((hr = dxgiFactory->EnumAdapters1(index, &newAdapter)) != DXGI_ERROR_NOT_FOUND)
    {
        if (FAILED(hr)) {
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to enumerate adapters");
        }

        adapters.push_back(newAdapter);
        newAdapter.Reset();
        ++index;
    }
}

} // unnamed namespace
//-----------------------------------------------------------------------
GraphicsDeviceDirect3D12::GraphicsDeviceDirect3D12()
{
    EnumerateAdapters(adapters);

    if (adapters.empty()) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "List of DXGI adapters is empty");
    }

    POMDOG_ASSERT(!adapters.empty());
    activeAdapter = adapters.front();

//    D3D12_CREATE_DEVICE_FLAG createDeviceFlags = D3D12_CREATE_DEVICE_NONE;
//#if defined(DEBUG) && !defined(NDEBUG)
//    createDeviceFlags |= D3D12_CREATE_DEVICE_DEBUG;
//#endif

    //constexpr D3D_DRIVER_TYPE driverTypes[] = {
    //    D3D_DRIVER_TYPE_HARDWARE,
    //    D3D_DRIVER_TYPE_WARP,
    //    D3D_DRIVER_TYPE_REFERENCE,
    //};

    constexpr D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    HRESULT hr = S_OK;

    for (auto featureLevel: featureLevels) {
        hr = D3D12CreateDevice(
            activeAdapter.Get(),
            featureLevel,
            IID_PPV_ARGS(&device));

        if (SUCCEEDED(hr)) {
            break;
        }
    }

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create ID3D12Device");
    }

    D3D12_FEATURE_DATA_D3D12_OPTIONS options;

    hr = device->CheckFeatureSupport(
        D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to check feature support");
    }
}
//-----------------------------------------------------------------------
GraphicsDeviceDirect3D12::~GraphicsDeviceDirect3D12()
{
    device.Reset();
    activeAdapter.Reset();
    adapters.clear();
}
//-----------------------------------------------------------------------
ShaderLanguage GraphicsDeviceDirect3D12::GetSupportedLanguage() const
{
    return ShaderLanguage::HLSL;
}
//-----------------------------------------------------------------------
std::unique_ptr<Shader>
GraphicsDeviceDirect3D12::CreateShader(ShaderBytecode const& shaderBytecode,
    ShaderCompileOptions const& compileOptions)
{
    auto shader = std::make_unique<ShaderDirect3D12>();
    auto & codeBlob = shader->CodeBlob;

    POMDOG_ASSERT(shaderBytecode.Code != nullptr);
    POMDOG_ASSERT(shaderBytecode.ByteLength > 0);

    if (compileOptions.Precompiled) {
        codeBlob.resize(shaderBytecode.ByteLength);
        std::memcpy(codeBlob.data(), shaderBytecode.Code, codeBlob.size());
    }
    else {
        auto compiledBlob = Direct3D::HLSLCompiling::CompileShader(
            shaderBytecode, compileOptions);
        POMDOG_ASSERT(compiledBlob.Get() != nullptr);

        codeBlob.resize(compiledBlob->GetBufferSize());
        std::memcpy(codeBlob.data(), compiledBlob->GetBufferPointer(), codeBlob.size());
    }

    return std::move(shader);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeBuffer>
GraphicsDeviceDirect3D12::CreateBuffer(
    std::size_t sizeInBytes, BufferUsage bufferUsage, BufferBindMode)
{
    POMDOG_ASSERT(device);
    return std::make_unique<BufferDirect3D12>(
        device.Get(), sizeInBytes, bufferUsage);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeBuffer>
GraphicsDeviceDirect3D12::CreateBuffer(
    void const* sourceData, std::size_t sizeInBytes,
    BufferUsage bufferUsage, BufferBindMode)
{
    POMDOG_ASSERT(device);
    return std::make_unique<BufferDirect3D12>(
        device.Get(), sourceData, sizeInBytes, bufferUsage);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeSamplerState>
GraphicsDeviceDirect3D12::CreateSamplerState(SamplerDescription const& description)
{
    POMDOG_ASSERT(device);
    return std::make_unique<SamplerStateDirect3D12>(device.Get(), description);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativePipelineState>
GraphicsDeviceDirect3D12::CreatePipelineState(
    PipelineStateDescription const& description)
{
    POMDOG_ASSERT(device);
    return std::make_unique<PipelineStateDirect3D12>(device.Get(), description);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeEffectReflection>
GraphicsDeviceDirect3D12::CreateEffectReflection(
    PipelineStateDescription const& description,
    NativePipelineState &)
{
    auto vertexShader = std::dynamic_pointer_cast<ShaderDirect3D12>(
        description.VertexShader);

    if (!vertexShader) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::domain_error,
            "Failed to cast to vertex shader");
    }

    auto pixelShader = std::dynamic_pointer_cast<ShaderDirect3D12>(
        description.PixelShader);

    if (!pixelShader) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::domain_error,
            "Failed to cast to pixel shader");
    }

    return std::make_unique<EffectReflectionDirect3D12>(
        vertexShader->GetShaderBytecode(),
        pixelShader->GetShaderBytecode());
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeTexture2D>
GraphicsDeviceDirect3D12::CreateTexture2D(std::int32_t width, std::int32_t height,
    std::int32_t mipmapLevels, SurfaceFormat format)
{
    POMDOG_ASSERT(device);
    return std::make_unique<Texture2DDirect3D12>(
        device.Get(), width, height, mipmapLevels, format);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeRenderTarget2D>
GraphicsDeviceDirect3D12::CreateRenderTarget2D(
    std::int32_t width,
    std::int32_t height,
    std::int32_t mipmapLevels,
    SurfaceFormat format,
    DepthFormat depthStencilFormat,
    std::int32_t multiSampleCount)
{
    POMDOG_ASSERT(device);
    return std::make_unique<RenderTarget2DDirect3D12>(
        device.Get(), width, height, mipmapLevels,
        format, depthStencilFormat, multiSampleCount);
}
//-----------------------------------------------------------------------
Microsoft::WRL::ComPtr<ID3D12Device>
GraphicsDeviceDirect3D12::GetDevice() const
{
    return device;
}
//-----------------------------------------------------------------------
} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog
