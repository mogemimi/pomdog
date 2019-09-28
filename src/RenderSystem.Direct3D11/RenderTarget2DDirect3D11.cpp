// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "RenderTarget2DDirect3D11.hpp"
#include "../RenderSystem.DXGI/DXGIFormatHelper.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog::Detail::Direct3D11 {
namespace {

using DXGI::DXGIFormatHelper;
using Microsoft::WRL::ComPtr;

void BuildRenderTarget(
    ID3D11Device* device,
    SurfaceFormat format,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    ComPtr<ID3D11Texture2D> & texture2D,
    ComPtr<ID3D11RenderTargetView> & renderTargetView,
    ComPtr<ID3D11ShaderResourceView> & textureResourceView)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount > 0);

    // Create a render texture
    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Format = DXGIFormatHelper::ToDXGIFormat(format);
    textureDesc.Width = pixelWidth;
    textureDesc.Height = pixelHeight;
    textureDesc.ArraySize = 1;
    textureDesc.MipLevels = levelCount;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    HRESULT hr = device->CreateTexture2D(&textureDesc, nullptr, &texture2D);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create render target texture");
    }

    // Create a render target view (RTV)
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    ZeroMemory(&rtvDesc, sizeof(rtvDesc));
    rtvDesc.Format = textureDesc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    POMDOG_ASSERT(texture2D);
    hr = device->CreateRenderTargetView(texture2D.Get(),
        &rtvDesc, &renderTargetView);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create RendetTargetView");
    }

    // Create shader resource view (SRV)
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;

    hr = device->CreateShaderResourceView(texture2D.Get(),
        &srvDesc, &textureResourceView);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create shader resource view");
    }
}

void BuildDepthBuffer(
    ID3D11Device* device,
    DepthFormat depthStencilFormat,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    ComPtr<ID3D11Texture2D> & depthStencil,
    ComPtr<ID3D11DepthStencilView> & depthStencilView)
{
    if (DepthFormat::None == depthStencilFormat) {
        return;
    }

    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount > 0);

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Format = DXGIFormatHelper::ToDXGIFormat(depthStencilFormat);
    textureDesc.Width = pixelWidth;
    textureDesc.Height = pixelHeight;
    textureDesc.ArraySize = 1;
    textureDesc.MipLevels = levelCount;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    HRESULT hr = device->CreateTexture2D(&textureDesc, nullptr, &depthStencil);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create depth stencil buffer");
    }

    // Create the depth stencil view (DSV)
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    dsvDesc.Format = textureDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    POMDOG_ASSERT(depthStencil);
    hr = device->CreateDepthStencilView(depthStencil.Get(), &dsvDesc, &depthStencilView);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create DepthStencilView");
    }
}

void BuildBackBufferBySwapChain(
    ID3D11Device* device,
    IDXGISwapChain* swapChain,
    ComPtr<ID3D11Texture2D> & texture2D,
    ComPtr<ID3D11RenderTargetView> & renderTargetView)
{
    POMDOG_ASSERT(device);
    POMDOG_ASSERT(swapChain);

    // Get a surface in the swap chain
    HRESULT hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&texture2D));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to call IDXGISwapChain::GetBuffer");
    }

    // Create a render target view
    POMDOG_ASSERT(texture2D);
    hr = device->CreateRenderTargetView(texture2D.Get(), nullptr, &renderTargetView);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create RenderTargetView");
    }
}

} // unnamed namespace

RenderTarget2DDirect3D11::RenderTarget2DDirect3D11(
    ID3D11Device* device,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format,
    DepthFormat depthStencilFormat,
    [[maybe_unused]] std::int32_t multiSampleCount)
{
    POMDOG_ASSERT(levelCount > 0);

    ///@todo MSAA is not implemented yet
    UNREFERENCED_PARAMETER(multiSampleCount);

    BuildRenderTarget(device, format, pixelWidth, pixelHeight, levelCount,
        texture2D, renderTargetView, textureResourceView);

    BuildDepthBuffer(device, depthStencilFormat, pixelWidth, pixelHeight,
        levelCount, depthStencil, depthStencilView);
}

RenderTarget2DDirect3D11::RenderTarget2DDirect3D11(
    ID3D11Device* device,
    IDXGISwapChain* swapChain,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    DepthFormat depthStencilFormat,
    [[maybe_unused]] std::int32_t multiSampleCount)
{
    ///@todo MSAA is not implemented yet
    UNREFERENCED_PARAMETER(multiSampleCount);

    constexpr std::int32_t backBufferMipLevels = 1;

    BuildBackBufferBySwapChain(device, swapChain,
        texture2D, renderTargetView);

    BuildDepthBuffer(device, depthStencilFormat, pixelWidth, pixelHeight,
        backBufferMipLevels, depthStencil, depthStencilView);
}

void RenderTarget2DDirect3D11::GetData(
    void* result,
    std::size_t offsetInBytes,
    std::size_t sizeInBytes,
    [[maybe_unused]] std::int32_t pixelWidth,
    [[maybe_unused]] std::int32_t pixelHeight,
    [[maybe_unused]] std::int32_t levelCount,
    [[maybe_unused]] SurfaceFormat format) const
{
    POMDOG_ASSERT(texture2D);

    // NOTE: Get the device context
    ComPtr<ID3D11Device> device;
    texture2D->GetDevice(&device);
    ComPtr<ID3D11DeviceContext> deviceContext;
    device->GetImmediateContext(&deviceContext);

    POMDOG_ASSERT(deviceContext != nullptr);

    // NOTE: Map the texture
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto hr = deviceContext->Map(
        texture2D.Get(),
        0,
        D3D11_MAP_READ,
        0,
        &mappedResource);

    ComPtr<ID3D11Texture2D> mappedTexture;

    if (!FAILED(hr)) {
        mappedTexture = texture2D;
    }
    else if (hr == E_INVALIDARG) {
        // NOTE: If we failed to map the texture, copy it to a staging resource.
        D3D11_TEXTURE2D_DESC desc;
        texture2D->GetDesc(&desc);

        desc.Usage = D3D11_USAGE_STAGING;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.MiscFlags = 0;

        ComPtr<ID3D11Texture2D> stagingTexture;
        hr = device->CreateTexture2D(&desc, nullptr, &stagingTexture);
        if (FAILED(hr)) {
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create staging texture");
        }

        // NOTE: Copy the texture to a staging resource.
        deviceContext->CopyResource(stagingTexture.Get(), texture2D.Get());

        // NOTE: Map the staging resource
        hr = deviceContext->Map(
            stagingTexture.Get(),
            0,
            D3D11_MAP_READ,
            0,
            &mappedResource);

        if (FAILED(hr)) {
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to map staging texture");
        }

        mappedTexture = std::move(stagingTexture);
    }
    else {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to map buffer");
    }

    POMDOG_ASSERT(result != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);
    std::memcpy(result, reinterpret_cast<const BYTE*>(mappedResource.pData) + offsetInBytes, sizeInBytes);

    POMDOG_ASSERT(mappedTexture != nullptr);
    deviceContext->Unmap(mappedTexture.Get(), 0);
}

ID3D11RenderTargetView* RenderTarget2DDirect3D11::GetRenderTargetView() const
{
    POMDOG_ASSERT(renderTargetView);
    return renderTargetView.Get();
}

ID3D11DepthStencilView* RenderTarget2DDirect3D11::GetDepthStencilView() const
{
    return depthStencilView.Get();
}

ID3D11ShaderResourceView* RenderTarget2DDirect3D11::GetShaderResourceView() const
{
    POMDOG_ASSERT(textureResourceView);
    return textureResourceView.Get();
}

void RenderTarget2DDirect3D11::ResetBackBuffer(
    ID3D11Device* device, IDXGISwapChain* swapChain,
    std::int32_t pixelWidth, std::int32_t pixelHeight,
    DepthFormat depthStencilFormat)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(swapChain != nullptr);

    renderTargetView.Reset();
    texture2D.Reset();
    depthStencilView.Reset();
    depthStencil.Reset();

    constexpr std::uint32_t backBufferMipLevels = 1;

    BuildBackBufferBySwapChain(device, swapChain,
        texture2D, renderTargetView);

    BuildDepthBuffer(device, depthStencilFormat, pixelWidth, pixelHeight,
        backBufferMipLevels, depthStencil, depthStencilView);
}

void RenderTarget2DDirect3D11::ResetBackBuffer()
{
    renderTargetView.Reset();
    texture2D.Reset();
}

} // namespace Pomdog::Detail::Direct3D11
