// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/direct3d11/depth_stencil_buffer_direct3d11.h"
#include "pomdog/gpu/dxgi/dxgi_format_helper.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail::direct3d11 {
namespace {

using Microsoft::WRL::ComPtr;

[[nodiscard]] std::unique_ptr<Error>
BuildDepthBuffer(
    ID3D11Device* device,
    PixelFormat depthStencilFormat,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    std::int32_t multiSampleCount,
    ComPtr<ID3D11Texture2D>& depthStencil,
    ComPtr<ID3D11DepthStencilView>& depthStencilView) noexcept
{
    if (depthStencilFormat == PixelFormat::Invalid) {
        return errors::New("depthStencilFormat must be != PixelFormat::Invalid");
    }
    if (pixelWidth <= 0) {
        return errors::New("pixelWidth must be > 0");
    }
    if (pixelHeight <= 0) {
        return errors::New("pixelHeight must be > 0");
    }
    if (levelCount <= 0) {
        return errors::New("levelCount must be > 0");
    }
    if (multiSampleCount <= 0) {
        return errors::New("multiSampleCount must be > 0");
    }

    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount > 0);
    POMDOG_ASSERT(multiSampleCount > 0);

    // NOTE: Create depth stencil texture
    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Format = dxgi::ToDXGIFormat(depthStencilFormat);
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
        return errors::New("CreateTexture2D() failed");
    }

    // NOTE: Create the depth stencil view (DSV)
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    dsvDesc.Format = textureDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    POMDOG_ASSERT(depthStencil != nullptr);

    if (auto hr = device->CreateDepthStencilView(depthStencil.Get(), &dsvDesc, &depthStencilView); FAILED(hr)) {
        return errors::New("CreateDepthStencilView() failed");
    }

    return nullptr;
}

} // namespace

std::unique_ptr<Error>
DepthStencilBufferDirect3D11::Initialize(
    ID3D11Device* device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    PixelFormat depthStencilFormatIn,
    std::int32_t multiSampleCount) noexcept
{
    pixelWidth = pixelWidthIn;
    pixelHeight = pixelHeightIn;
    depthStencilFormat = depthStencilFormatIn;
    multiSampleEnabled = (multiSampleCount > 1);

    constexpr std::int32_t levelCount = 1;

    if (auto err = BuildDepthBuffer(
            device,
            depthStencilFormat,
            pixelWidth,
            pixelHeight,
            levelCount,
            multiSampleCount,
            depthStencil,
            depthStencilView);
        err != nullptr) {
        return errors::Wrap(std::move(err), "BuildDepthBuffer() failed");
    }

    return nullptr;
}

std::int32_t DepthStencilBufferDirect3D11::GetWidth() const noexcept
{
    return pixelWidth;
}

std::int32_t DepthStencilBufferDirect3D11::GetHeight() const noexcept
{
    return pixelHeight;
}

PixelFormat DepthStencilBufferDirect3D11::GetFormat() const noexcept
{
    return depthStencilFormat;
}

Rectangle DepthStencilBufferDirect3D11::GetBounds() const noexcept
{
    return Rectangle{0, 0, pixelWidth, pixelHeight};
}

ID3D11DepthStencilView*
DepthStencilBufferDirect3D11::GetDepthStencilView() const noexcept
{
    return depthStencilView.Get();
}

std::unique_ptr<Error>
DepthStencilBufferDirect3D11::ResetBuffer(
    ID3D11Device* device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    PixelFormat depthStencilFormatIn,
    std::int32_t multiSampleCount) noexcept
{
    POMDOG_ASSERT(device != nullptr);

    pixelWidth = pixelWidthIn;
    pixelHeight = pixelHeightIn;
    depthStencilFormat = depthStencilFormatIn;
    multiSampleEnabled = (multiSampleCount > 1);

    depthStencilView.Reset();
    depthStencil.Reset();

    constexpr std::int32_t levelCount = 1;

    if (auto err = BuildDepthBuffer(
            device,
            depthStencilFormat,
            pixelWidth,
            pixelHeight,
            levelCount,
            multiSampleCount,
            depthStencil,
            depthStencilView);
        err != nullptr) {
        return errors::Wrap(std::move(err), "BuildDepthBuffer() failed");
    }

    return nullptr;
}

} // namespace pomdog::gpu::detail::direct3d11
