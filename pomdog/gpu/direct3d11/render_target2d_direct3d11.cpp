// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/direct3d11/render_target2d_direct3d11.h"
#include "pomdog/gpu/dxgi/dxgi_format_helper.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/exception.h"

namespace pomdog::gpu::detail::direct3d11 {
namespace {

using Microsoft::WRL::ComPtr;

[[nodiscard]] std::unique_ptr<Error>
buildRenderTarget(
    unsafe_ptr<ID3D11Device> device,
    PixelFormat format,
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
    textureDesc.Format = dxgi::toDXGIFormat(format);
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
        return errors::make("CreateTexture2D() failed");
    }

    // NOTE: Create a render target view (RTV)
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    ZeroMemory(&rtvDesc, sizeof(rtvDesc));
    rtvDesc.Format = textureDesc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    POMDOG_ASSERT(texture2D);
    if (auto hr = device->CreateRenderTargetView(texture2D.Get(), &rtvDesc, &renderTargetView); FAILED(hr)) {
        return errors::make("CreateRenderTargetView() failed");
    }

    // NOTE: Create shader resource view (SRV)
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;

    if (auto hr = device->CreateShaderResourceView(texture2D.Get(), &srvDesc, &textureResourceView); FAILED(hr)) {
        return errors::make("CreateShaderResourceView() failed");
    }

    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
buildBackBufferBySwapChain(
    unsafe_ptr<ID3D11Device> device,
    unsafe_ptr<IDXGISwapChain> swapChain,
    ComPtr<ID3D11Texture2D>& texture2D,
    ComPtr<ID3D11RenderTargetView>& renderTargetView) noexcept
{
    POMDOG_ASSERT(device);
    POMDOG_ASSERT(swapChain);

    // NOTE: Get a surface in the swap chain
    if (auto hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&texture2D)); FAILED(hr)) {
        return errors::make("IDXGISwapChain::GetBuffer() failed");
    }

    // NOTE: Create a render target view
    POMDOG_ASSERT(texture2D != nullptr);
    if (auto hr = device->CreateRenderTargetView(texture2D.Get(), nullptr, &renderTargetView); FAILED(hr)) {
        return errors::make("CreateRenderTargetView() failed");
    }

    return nullptr;
}

} // namespace

std::unique_ptr<Error>
RenderTarget2DDirect3D11::initialize(
    unsafe_ptr<ID3D11Device> device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    std::int32_t levelCountIn,
    PixelFormat formatIn,
    std::int32_t multiSampleCount) noexcept
{
    pixelWidth_ = pixelWidthIn;
    pixelHeight_ = pixelHeightIn;
    levelCount_ = levelCountIn;
    format_ = formatIn;
    generateMipmap_ = (levelCountIn > 1);
    multiSampleEnabled_ = (multiSampleCount > 1);

    POMDOG_ASSERT(levelCount_ > 0);

    if (auto err = buildRenderTarget(
            device,
            format_,
            pixelWidth_,
            pixelHeight_,
            levelCount_,
            texture2D_,
            renderTargetView_,
            textureResourceView_);
        err != nullptr) {
        return errors::wrap(std::move(err), "BuildRenderTarget() failed");
    }

    return nullptr;
}

std::unique_ptr<Error>
RenderTarget2DDirect3D11::initialize(
    unsafe_ptr<ID3D11Device> device,
    unsafe_ptr<IDXGISwapChain> swapChain,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    std::int32_t levelCountIn,
    PixelFormat formatIn,
    std::int32_t multiSampleCount) noexcept
{
    pixelWidth_ = pixelWidthIn;
    pixelHeight_ = pixelHeightIn;
    levelCount_ = levelCountIn;
    format_ = formatIn;
    generateMipmap_ = (levelCountIn > 1);
    multiSampleEnabled_ = (multiSampleCount > 1);

    if (auto err = buildBackBufferBySwapChain(
            device,
            swapChain,
            texture2D_,
            renderTargetView_);
        err != nullptr) {
        return errors::wrap(std::move(err), "BuildBackBufferBySwapChain() failed");
    }

    return nullptr;
}

std::int32_t
RenderTarget2DDirect3D11::getWidth() const noexcept
{
    return pixelWidth_;
}

std::int32_t
RenderTarget2DDirect3D11::getHeight() const noexcept
{
    return pixelHeight_;
}

std::int32_t
RenderTarget2DDirect3D11::getLevelCount() const noexcept
{
    return levelCount_;
}

PixelFormat
RenderTarget2DDirect3D11::getFormat() const noexcept
{
    return format_;
}

Rectangle
RenderTarget2DDirect3D11::getBounds() const noexcept
{
    return Rectangle{0, 0, pixelWidth_, pixelHeight_};
}

void RenderTarget2DDirect3D11::getData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const
{
    POMDOG_ASSERT(texture2D_);

    // NOTE: Get the device context
    ComPtr<ID3D11Device> device;
    texture2D_->GetDevice(&device);
    ComPtr<ID3D11DeviceContext> deviceContext;
    device->GetImmediateContext(&deviceContext);

    POMDOG_ASSERT(deviceContext != nullptr);

    // NOTE: Map the texture
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto hr = deviceContext->Map(
        texture2D_.Get(),
        0,
        D3D11_MAP_READ,
        0,
        &mappedResource);

    ComPtr<ID3D11Texture2D> mappedTexture;

    if (!FAILED(hr)) {
        mappedTexture = texture2D_;
    }
    else if (hr == E_INVALIDARG) {
        // NOTE: If we failed to map the texture, copy it to a staging resource.
        D3D11_TEXTURE2D_DESC desc;
        texture2D_->GetDesc(&desc);

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
        deviceContext->CopyResource(stagingTexture.Get(), texture2D_.Get());

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

unsafe_ptr<ID3D11RenderTargetView>
RenderTarget2DDirect3D11::getRenderTargetView() const noexcept
{
    POMDOG_ASSERT(renderTargetView_);
    return renderTargetView_.Get();
}

unsafe_ptr<ID3D11ShaderResourceView>
RenderTarget2DDirect3D11::getShaderResourceView() const noexcept
{
    POMDOG_ASSERT(textureResourceView_);
    return textureResourceView_.Get();
}

std::unique_ptr<Error>
RenderTarget2DDirect3D11::resetBackBuffer(
    ID3D11Device* device,
    IDXGISwapChain* swapChain,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(swapChain != nullptr);

    pixelWidth_ = pixelWidthIn;
    pixelHeight_ = pixelHeightIn;

    renderTargetView_.Reset();
    texture2D_.Reset();

    if (auto err = buildBackBufferBySwapChain(
            device,
            swapChain,
            texture2D_,
            renderTargetView_);
        err != nullptr) {
        return errors::wrap(std::move(err), "buildBackBufferBySwapChain() failed");
    }

    return nullptr;
}

void RenderTarget2DDirect3D11::resetBackBuffer() noexcept
{
    renderTargetView_.Reset();
    texture2D_.Reset();
}

} // namespace pomdog::gpu::detail::direct3d11
