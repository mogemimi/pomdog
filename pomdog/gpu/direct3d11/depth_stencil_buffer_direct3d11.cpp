// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/direct3d11/depth_stencil_buffer_direct3d11.h"
#include "pomdog/gpu/dxgi/dxgi_format_helper.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail::direct3d11 {
namespace {

using Microsoft::WRL::ComPtr;

[[nodiscard]] std::unique_ptr<Error>
buildDepthBuffer(
    unsafe_ptr<ID3D11Device> device,
    PixelFormat depthStencilFormat,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    std::int32_t multiSampleCount,
    ComPtr<ID3D11Texture2D>& depthStencil,
    ComPtr<ID3D11DepthStencilView>& depthStencilView) noexcept
{
    if (depthStencilFormat == PixelFormat::Invalid) {
        return errors::make("depthStencilFormat must be != PixelFormat::Invalid");
    }
    if (pixelWidth <= 0) {
        return errors::make("pixelWidth must be > 0");
    }
    if (pixelHeight <= 0) {
        return errors::make("pixelHeight must be > 0");
    }
    if (levelCount <= 0) {
        return errors::make("levelCount must be > 0");
    }
    if (multiSampleCount <= 0) {
        return errors::make("multiSampleCount must be > 0");
    }

    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount > 0);
    POMDOG_ASSERT(multiSampleCount > 0);

    // NOTE: Create depth stencil texture
    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Format = dxgi::toDXGIFormat(depthStencilFormat);
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

    if (auto hr = device->CreateTexture2D(&textureDesc, nullptr, &depthStencil); FAILED(hr)) {
        return errors::make("CreateTexture2D() failed");
    }

    // NOTE: Create the depth stencil view (DSV)
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    dsvDesc.Format = textureDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    POMDOG_ASSERT(depthStencil != nullptr);

    if (auto hr = device->CreateDepthStencilView(depthStencil.Get(), &dsvDesc, &depthStencilView); FAILED(hr)) {
        return errors::make("CreateDepthStencilView() failed");
    }

    return nullptr;
}

} // namespace

std::unique_ptr<Error>
DepthStencilBufferDirect3D11::initialize(
    unsafe_ptr<ID3D11Device> device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    PixelFormat depthStencilFormatIn,
    std::int32_t multiSampleCount) noexcept
{
    pixelWidth_ = pixelWidthIn;
    pixelHeight_ = pixelHeightIn;
    depthStencilFormat_ = depthStencilFormatIn;
    multiSampleEnabled_ = (multiSampleCount > 1);

    constexpr std::int32_t levelCount = 1;

    if (auto err = buildDepthBuffer(
            device,
            depthStencilFormat_,
            pixelWidth_,
            pixelHeight_,
            levelCount,
            multiSampleCount,
            depthStencil_,
            depthStencilView_);
        err != nullptr) {
        return errors::wrap(std::move(err), "BuildDepthBuffer() failed");
    }

    return nullptr;
}

std::int32_t
DepthStencilBufferDirect3D11::getWidth() const noexcept
{
    return pixelWidth_;
}

std::int32_t
DepthStencilBufferDirect3D11::getHeight() const noexcept
{
    return pixelHeight_;
}

PixelFormat
DepthStencilBufferDirect3D11::getFormat() const noexcept
{
    return depthStencilFormat_;
}

Rectangle
DepthStencilBufferDirect3D11::getBounds() const noexcept
{
    return Rectangle{0, 0, pixelWidth_, pixelHeight_};
}

ID3D11DepthStencilView*
DepthStencilBufferDirect3D11::getDepthStencilView() const noexcept
{
    return depthStencilView_.Get();
}

std::unique_ptr<Error>
DepthStencilBufferDirect3D11::resetBuffer(
    unsafe_ptr<ID3D11Device> device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    PixelFormat depthStencilFormatIn,
    std::int32_t multiSampleCount) noexcept
{
    POMDOG_ASSERT(device != nullptr);

    pixelWidth_ = pixelWidthIn;
    pixelHeight_ = pixelHeightIn;
    depthStencilFormat_ = depthStencilFormatIn;
    multiSampleEnabled_ = (multiSampleCount > 1);

    depthStencilView_.Reset();
    depthStencil_.Reset();

    constexpr std::int32_t levelCount = 1;

    if (auto err = buildDepthBuffer(
            device,
            depthStencilFormat_,
            pixelWidth_,
            pixelHeight_,
            levelCount,
            multiSampleCount,
            depthStencil_,
            depthStencilView_);
        err != nullptr) {
        return errors::wrap(std::move(err), "BuildDepthBuffer() failed");
    }

    return nullptr;
}

} // namespace pomdog::gpu::detail::direct3d11
