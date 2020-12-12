// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "RenderTarget2DDirect3D11.hpp"
#include "../Graphics.DXGI/DXGIFormatHelper.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog::Detail::Direct3D11 {
namespace {

using DXGI::DXGIFormatHelper;
using Microsoft::WRL::ComPtr;

[[nodiscard]] std::shared_ptr<Error>
BuildRenderTarget(
    ID3D11Device* device,
    SurfaceFormat format,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    ComPtr<ID3D11Texture2D>& texture2D,
    ComPtr<ID3D11RenderTargetView>& renderTargetView,
    ComPtr<ID3D11ShaderResourceView>& textureResourceView) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount > 0);

    // NOTE: Create a render texture
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

    if (auto hr = device->CreateTexture2D(&textureDesc, nullptr, &texture2D); FAILED(hr)) {
        return Errors::New("CreateTexture2D() failed");
    }

    // NOTE: Create a render target view (RTV)
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    ZeroMemory(&rtvDesc, sizeof(rtvDesc));
    rtvDesc.Format = textureDesc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    POMDOG_ASSERT(texture2D);
    if (auto hr = device->CreateRenderTargetView(texture2D.Get(), &rtvDesc, &renderTargetView); FAILED(hr)) {
        return Errors::New("CreateRenderTargetView() failed");
    }

    // NOTE: Create shader resource view (SRV)
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;

    if (auto hr = device->CreateShaderResourceView(texture2D.Get(), &srvDesc, &textureResourceView); FAILED(hr)) {
        return Errors::New("CreateShaderResourceView() failed");
    }

    return nullptr;
}

[[nodiscard]] std::shared_ptr<Error>
BuildBackBufferBySwapChain(
    ID3D11Device* device,
    IDXGISwapChain* swapChain,
    ComPtr<ID3D11Texture2D>& texture2D,
    ComPtr<ID3D11RenderTargetView>& renderTargetView) noexcept
{
    POMDOG_ASSERT(device);
    POMDOG_ASSERT(swapChain);

    // NOTE: Get a surface in the swap chain
    if (auto hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&texture2D)); FAILED(hr)) {
        return Errors::New("IDXGISwapChain::GetBuffer() failed");
    }

    // NOTE: Create a render target view
    POMDOG_ASSERT(texture2D != nullptr);
    if (auto hr = device->CreateRenderTargetView(texture2D.Get(), nullptr, &renderTargetView); FAILED(hr)) {
        return Errors::New("CreateRenderTargetView() failed");
    }

    return nullptr;
}

} // namespace

std::shared_ptr<Error>
RenderTarget2DDirect3D11::Initialize(
    ID3D11Device* device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    std::int32_t levelCountIn,
    SurfaceFormat formatIn,
    DepthFormat depthStencilFormat,
    std::int32_t multiSampleCount) noexcept
{
    pixelWidth = pixelWidthIn;
    pixelHeight = pixelHeightIn;
    levelCount = levelCountIn;
    format = formatIn;
    generateMipmap = (levelCountIn > 1);
    multiSampleEnabled = (multiSampleCount > 1);

    POMDOG_ASSERT(levelCount > 0);

    if (auto err = BuildRenderTarget(
            device,
            format,
            pixelWidth,
            pixelHeight,
            levelCount,
            texture2D,
            renderTargetView,
            textureResourceView);
        err != nullptr) {
        return Errors::Wrap(std::move(err), "BuildRenderTarget() failed");
    }

    if (depthStencilFormat != DepthFormat::None) {
        if (auto err = depthStencilBuffer.Initialize(
            device,
            pixelWidth,
            pixelHeight,
            depthStencilFormat,
            multiSampleCount);
            err != nullptr) {
            return Errors::Wrap(std::move(err), "depthStencilBuffer.Initialize() failed");
        }
    }

    return nullptr;
}

std::shared_ptr<Error>
RenderTarget2DDirect3D11::Initialize(
    ID3D11Device* device,
    IDXGISwapChain* swapChain,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    std::int32_t levelCountIn,
    SurfaceFormat formatIn,
    DepthFormat depthStencilFormat,
    std::int32_t multiSampleCount) noexcept
{
    pixelWidth = pixelWidthIn;
    pixelHeight = pixelHeightIn;
    levelCount = levelCountIn;
    format = formatIn;
    generateMipmap = (levelCountIn > 1);
    multiSampleEnabled = (multiSampleCount > 1);

    if (auto err = BuildBackBufferBySwapChain(
            device,
            swapChain,
            texture2D,
            renderTargetView);
        err != nullptr) {
        return Errors::Wrap(std::move(err), "BuildBackBufferBySwapChain() failed");
    }

    if (depthStencilFormat != DepthFormat::None) {
        if (auto err = depthStencilBuffer.Initialize(
            device,
            pixelWidth,
            pixelHeight,
            depthStencilFormat,
            multiSampleCount);
            err != nullptr) {
            return Errors::Wrap(std::move(err), "depthStencilBuffer.Initialize() failed");
        }
    }

    return nullptr;
}

std::int32_t RenderTarget2DDirect3D11::GetWidth() const noexcept
{
    return pixelWidth;
}

std::int32_t RenderTarget2DDirect3D11::GetHeight() const noexcept
{
    return pixelHeight;
}

std::int32_t RenderTarget2DDirect3D11::GetLevelCount() const noexcept
{
    return levelCount;
}

SurfaceFormat RenderTarget2DDirect3D11::GetFormat() const noexcept
{
    return format;
}

DepthFormat RenderTarget2DDirect3D11::GetDepthStencilFormat() const noexcept
{
    return depthStencilBuffer.GetFormat();
}

Rectangle RenderTarget2DDirect3D11::GetBounds() const noexcept
{
    return Rectangle{0, 0, pixelWidth, pixelHeight};
}

void RenderTarget2DDirect3D11::GetData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const
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

ID3D11RenderTargetView*
RenderTarget2DDirect3D11::GetRenderTargetView() const noexcept
{
    POMDOG_ASSERT(renderTargetView);
    return renderTargetView.Get();
}

ID3D11DepthStencilView*
RenderTarget2DDirect3D11::GetDepthStencilView() const noexcept
{
    return depthStencilBuffer.GetDepthStencilView();
}

ID3D11ShaderResourceView*
RenderTarget2DDirect3D11::GetShaderResourceView() const noexcept
{
    POMDOG_ASSERT(textureResourceView);
    return textureResourceView.Get();
}

std::shared_ptr<Error>
RenderTarget2DDirect3D11::ResetBackBuffer(
    ID3D11Device* device,
    IDXGISwapChain* swapChain,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    DepthFormat depthStencilFormat) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(swapChain != nullptr);

    pixelWidth = pixelWidthIn;
    pixelHeight = pixelHeightIn;

    renderTargetView.Reset();
    texture2D.Reset();
    depthStencilBuffer.ResetBackBuffer();

    if (auto err = BuildBackBufferBySwapChain(
            device,
            swapChain,
            texture2D,
            renderTargetView);
        err != nullptr) {
        return Errors::Wrap(std::move(err), "BuildBackBufferBySwapChain() failed");
    }

    constexpr std::int32_t multiSampleCount = 1;

    if (depthStencilFormat != DepthFormat::None) {
        if (auto err = depthStencilBuffer.ResetBackBuffer(
            device,
            pixelWidth,
            pixelHeight,
            depthStencilFormat,
            multiSampleCount);
            err != nullptr) {
            return Errors::Wrap(std::move(err), "depthStencilBuffer.ResetBackBuffer() failed");
        }
    }

    return nullptr;
}

void RenderTarget2DDirect3D11::ResetBackBuffer() noexcept
{
    renderTargetView.Reset();
    texture2D.Reset();
    depthStencilBuffer.ResetBackBuffer();
}

} // namespace Pomdog::Detail::Direct3D11
