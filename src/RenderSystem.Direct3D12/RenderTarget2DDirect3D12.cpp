// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "RenderTarget2DDirect3D12.hpp"
#include "../RenderSystem.DXGI/DXGIFormatHelper.hpp"
#include "../RenderSystem/TextureHelper.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <limits>
#include <algorithm>

namespace Pomdog {
namespace Detail {
namespace Direct3D12 {
namespace {

using DXGI::DXGIFormatHelper;

static void CreateDepthStencilBuffer(
    ID3D12Device* device,
    DepthFormat depthStencilFormat,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    UINT16 mipLevels,
    UINT sampleCount,
    UINT sampleQuality,
    Microsoft::WRL::ComPtr<ID3D12Resource> & depthBuffer)
{
    auto format = DXGIFormatHelper::ToDXGIFormat(depthStencilFormat);

    D3D12_HEAP_PROPERTIES heapProps = CD3D12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_RESOURCE_DESC resourceDesc = CD3D12_RESOURCE_DESC::Tex2D(
        format,
        pixelWidth, pixelHeight, 1, mipLevels, sampleCount, sampleQuality,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_TEXTURE_LAYOUT_UNKNOWN);
    D3D12_CLEAR_VALUE clearValue = CD3D12_CLEAR_VALUE(format, 1.0f, 0);

    // Create depth stencil buffer
    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(&depthBuffer));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create depth stencil buffer");
    }
}
//-----------------------------------------------------------------------
static D3D12_DSV_FLAGS ToDSVFlags(DepthFormat format) noexcept
{
    switch (format) {
    case DepthFormat::Depth16:
    case DepthFormat::Depth32:
        return D3D12_DSV_FLAG_READ_ONLY_DEPTH;
    default:
        break;
    }
    return D3D12_DSV_FLAG_NONE;
}
//-----------------------------------------------------------------------
static void CreateDepthStencilView(
    ID3D12Device* device,
    DepthFormat depthStencilFormat,
    Microsoft::WRL::ComPtr<ID3D12Resource> const& depthBuffer,
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> & depthStencilViewHeap)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(depthStencilFormat != DepthFormat::None);
    POMDOG_ASSERT(depthBuffer);

    // Create descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDesc.NodeMask = 0;
    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

    HRESULT hr = device->CreateDescriptorHeap(
        &heapDesc, IID_PPV_ARGS(&depthStencilViewHeap));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create descriptor heap");
    }

    // Create depth stencil view (DSV)
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    dsvDesc.Flags = ToDSVFlags(depthStencilFormat);
    dsvDesc.Format = DXGIFormatHelper::ToDXGIFormat(depthStencilFormat);
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    POMDOG_ASSERT(depthStencilViewHeap);
    auto depthStencilView = depthStencilViewHeap->GetCPUDescriptorHandleForHeapStart();

    POMDOG_ASSERT(depthBuffer);
    device->CreateDepthStencilView(
        depthBuffer.Get(), &dsvDesc, depthStencilView);

#ifdef DEBUG
    depthBuffer->SetName(L"Pomdog.DepthBuffer");
#endif
}
//-----------------------------------------------------------------------
static void CreateShaderResourceView(
    ID3D12Device* device,
    DXGI_FORMAT dxgiFormat,
    UINT16 mipLevels,
    Microsoft::WRL::ComPtr<ID3D12Resource> const& texture2D,
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> & shaderResourceViewHeap)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(texture2D);

    // Create descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    heapDesc.NodeMask = 0;
    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

    HRESULT hr = device->CreateDescriptorHeap(
        &heapDesc, IID_PPV_ARGS(&shaderResourceViewHeap));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create descriptor heap");
    }

    // Create shader resource view (SRV)
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = dxgiFormat;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = mipLevels;
    srvDesc.Texture2D.PlaneSlice = 0;

    ///@todo FIXME
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.ResourceMinLODClamp = 0;

    POMDOG_ASSERT(shaderResourceViewHeap);

    device->CreateShaderResourceView(
        texture2D.Get(),
        &srvDesc,
        shaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart());
}

} // unnamed namespace
//-----------------------------------------------------------------------
RenderTarget2DDirect3D12::RenderTarget2DDirect3D12(
    ID3D12Device* device,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format,
    DepthFormat depthStencilFormat,
    std::int32_t multiSampleCount)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 0);

    ///@todo MSAA is not implemnted yet
    POMDOG_ASSERT(multiSampleCount >= 0);
    POMDOG_ASSERT(multiSampleCount <= 1);
    UNREFERENCED_PARAMETER(multiSampleCount);

    constexpr UINT sampleCount = 1;
    constexpr UINT sampleQuality = 0;

    auto mipLevels = static_cast<UINT16>(std::min<decltype(levelCount)>(
        levelCount,
        std::numeric_limits<UINT16>::max()));

    {
        auto dxgiFormat = DXGIFormatHelper::ToDXGIFormat(format);
        D3D12_HEAP_PROPERTIES heapProps = CD3D12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_RESOURCE_DESC resourceDesc = CD3D12_RESOURCE_DESC::Tex2D(
            dxgiFormat,
            pixelWidth, pixelHeight, 1, mipLevels, sampleCount, sampleQuality,
            D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_TEXTURE_LAYOUT_UNKNOWN);

        const FLOAT clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

        D3D12_CLEAR_VALUE clearValue = CD3D12_CLEAR_VALUE(dxgiFormat, clearColor);

        // Create texture2D
        HRESULT hr = device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            &clearValue,
            IID_PPV_ARGS(&texture2D));

        if (FAILED(hr)) {
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to create texture2d");
        }

#ifdef DEBUG
        texture2D->SetName(L"Pomdog.RenderTarget2D");
#endif

        // Create descriptor heap
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        heapDesc.NodeMask = 0;
        heapDesc.NumDescriptors = 1;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

        hr = device->CreateDescriptorHeap(
            &heapDesc, IID_PPV_ARGS(&renderTargetViewHeap));

        if (FAILED(hr)) {
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to create descriptor heap");
        }

        // Create render target view (RTV)
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
        ZeroMemory(&rtvDesc, sizeof(rtvDesc));
        rtvDesc.Format = resourceDesc.Format;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0;
        rtvDesc.Texture2D.PlaneSlice = 0;

        POMDOG_ASSERT(renderTargetViewHeap);
        auto renderTargetView = renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();

        POMDOG_ASSERT(texture2D);
        device->CreateRenderTargetView(texture2D.Get(), &rtvDesc, renderTargetView);

        // Create shader resource view (SRV)
        CreateShaderResourceView(
            device, dxgiFormat, mipLevels, texture2D, shaderResourceViewHeap);
    }

    if (depthStencilFormat != DepthFormat::None)
    {
        CreateDepthStencilBuffer(device, depthStencilFormat,
            pixelWidth, pixelHeight, mipLevels, sampleCount, sampleQuality,
            depthBuffer);

        // Create depth stencil view (DSV)
        CreateDepthStencilView(
            device, depthStencilFormat, depthBuffer, depthStencilViewHeap);
    }
}
//-----------------------------------------------------------------------
RenderTarget2DDirect3D12::RenderTarget2DDirect3D12(
    ID3D12Device* device,
    IDXGISwapChain* swapChain,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    DXGI_FORMAT dxgiFormat,
    DepthFormat depthStencilFormat,
    std::int32_t multiSampleCount)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);

    ///@todo MSAA is not implemnted yet
    POMDOG_ASSERT(multiSampleCount >= 0);
    POMDOG_ASSERT(multiSampleCount <= 1);
    UNREFERENCED_PARAMETER(multiSampleCount);

    constexpr UINT16 mipLevels = 1;
    constexpr UINT sampleCount = 1;
    constexpr UINT sampleQuality = 0;

    // Get a texture 2D in the swap chain
    HRESULT hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&texture2D));
    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to get texture buffer from swap chain");
    }

    {
        // Create descriptor heap
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        heapDesc.NodeMask = 0;
        heapDesc.NumDescriptors = 1;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

        hr = device->CreateDescriptorHeap(
            &heapDesc, IID_PPV_ARGS(&renderTargetViewHeap));

        if (FAILED(hr)) {
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to create descriptor heap");
        }

        // Create render target view (RTV)
        POMDOG_ASSERT(renderTargetViewHeap);
        auto renderTargetView = renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();

        POMDOG_ASSERT(texture2D);
        device->CreateRenderTargetView(texture2D.Get(), nullptr, renderTargetView);

        // Create shader resource view (SRV)
        CreateShaderResourceView(
            device, dxgiFormat, mipLevels, texture2D, shaderResourceViewHeap);
    }

    if (depthStencilFormat != DepthFormat::None)
    {
        // Create depth stencil buffer
        CreateDepthStencilBuffer(device, depthStencilFormat,
            pixelWidth, pixelHeight, mipLevels, sampleCount, sampleQuality,
            depthBuffer);

        // Create depth stencil view (DSV)
        CreateDepthStencilView(
            device, depthStencilFormat, depthBuffer, depthStencilViewHeap);
    }
}
//-----------------------------------------------------------------------
ID3D12DescriptorHeap* RenderTarget2DDirect3D12::GetRenderTargetDescriptorHeap() const
{
    POMDOG_ASSERT(renderTargetViewHeap);
    return renderTargetViewHeap.Get();
}
//-----------------------------------------------------------------------
ID3D12DescriptorHeap* RenderTarget2DDirect3D12::GetDepthStencilDescriptorHeap() const
{
    //POMDOG_ASSERT(depthStencilViewHeap);
    if (depthStencilViewHeap) {
        return depthStencilViewHeap.Get();
    }
    return nullptr;
}
//-----------------------------------------------------------------------
ID3D12DescriptorHeap* RenderTarget2DDirect3D12::GetShaderResourceViewHeap() const
{
    POMDOG_ASSERT(shaderResourceViewHeap);
    return shaderResourceViewHeap.Get();
}
//-----------------------------------------------------------------------
ID3D12Resource* RenderTarget2DDirect3D12::GetTexture2D() const
{
    POMDOG_ASSERT(texture2D);
    return texture2D.Get();
}
//-----------------------------------------------------------------------
void RenderTarget2DDirect3D12::ResetBackBuffer(
    ID3D12Device* device,
    IDXGISwapChain* swapChain,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    DepthFormat depthStencilFormat)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(swapChain != nullptr);

    renderTargetViewHeap.Reset();
    texture2D.Reset();
    depthStencilViewHeap.Reset();
    depthBuffer.Reset();

    constexpr UINT16 mipLevels = 1;
    constexpr UINT sampleCount = 1;
    constexpr UINT sampleQuality = 0;

    // Get a texture 2D in the swap chain
    HRESULT hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&texture2D));
    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to get texture buffer from swap chain");
    }

    {
        // Create descriptor heap
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        heapDesc.NodeMask = 0;
        heapDesc.NumDescriptors = 1;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

        hr = device->CreateDescriptorHeap(
            &heapDesc, IID_PPV_ARGS(&renderTargetViewHeap));

        if (FAILED(hr)) {
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to create descriptor heap");
        }

        // Create render target view (RTV)
        POMDOG_ASSERT(renderTargetViewHeap);
        auto renderTargetView = renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();

        POMDOG_ASSERT(texture2D);
        device->CreateRenderTargetView(texture2D.Get(), nullptr, renderTargetView);
    }

    if (depthStencilFormat != DepthFormat::None)
    {
        CreateDepthStencilBuffer(device, depthStencilFormat,
            pixelWidth, pixelHeight, mipLevels, sampleCount, sampleQuality,
            depthBuffer);

        CreateDepthStencilView(
            device, depthStencilFormat, depthBuffer, depthStencilViewHeap);
    }
}
//-----------------------------------------------------------------------
void RenderTarget2DDirect3D12::ResetBackBuffer()
{
    renderTargetViewHeap.Reset();
    texture2D.Reset();
}
//-----------------------------------------------------------------------
} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog
