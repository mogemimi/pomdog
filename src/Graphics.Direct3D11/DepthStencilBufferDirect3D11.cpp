// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "DepthStencilBufferDirect3D11.hpp"
#include "../Graphics.DXGI/DXGIFormatHelper.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog::Detail::Direct3D11 {
namespace {

using DXGI::DXGIFormatHelper;
using Microsoft::WRL::ComPtr;

[[nodiscard]] std::shared_ptr<Error>
BuildDepthBuffer(
    ID3D11Device* device,
    DepthFormat depthStencilFormat,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    std::int32_t multiSampleCount,
    ComPtr<ID3D11Texture2D>& depthStencil,
    ComPtr<ID3D11DepthStencilView>& depthStencilView) noexcept
{
    if (depthStencilFormat == DepthFormat::None) {
        return Errors::New("depthStencilFormat must be != DepthFormat::None");
    }
    if (pixelWidth <= 0) {
        return Errors::New("pixelWidth must be > 0");
    }
    if (pixelHeight <= 0) {
        return Errors::New("pixelHeight must be > 0");
    }
    if (levelCount <= 0) {
        return Errors::New("levelCount must be > 0");
    }
    if (multiSampleCount <= 0) {
        return Errors::New("multiSampleCount must be > 0");
    }

    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount > 0);
    POMDOG_ASSERT(multiSampleCount > 0);

    // NOTE: Create depth stencil texture
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

    if (auto hr = device->CreateTexture2D(&textureDesc, nullptr, &depthStencil); FAILED(hr)) {
        return Errors::New("CreateTexture2D() failed");
    }

    // NOTE: Create the depth stencil view (DSV)
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    dsvDesc.Format = textureDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    POMDOG_ASSERT(depthStencil != nullptr);

    if (auto hr = device->CreateDepthStencilView(depthStencil.Get(), &dsvDesc, &depthStencilView); FAILED(hr)) {
        return Errors::New("CreateDepthStencilView() failed");
    }

    return nullptr;
}

} // namespace

std::shared_ptr<Error>
DepthStencilBufferDirect3D11::Initialize(
    ID3D11Device* device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    DepthFormat depthStencilFormatIn,
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
        return Errors::Wrap(std::move(err), "BuildDepthBuffer() failed");
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

DepthFormat DepthStencilBufferDirect3D11::GetFormat() const noexcept
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

std::shared_ptr<Error>
DepthStencilBufferDirect3D11::ResetBackBuffer(
    ID3D11Device* device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    DepthFormat depthStencilFormatIn,
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
        return Errors::Wrap(std::move(err), "BuildDepthBuffer() failed");
    }

    return nullptr;
}

void DepthStencilBufferDirect3D11::ResetBackBuffer() noexcept
{
    depthStencilView.Reset();
    depthStencil.Reset();
}

} // namespace Pomdog::Detail::Direct3D11
