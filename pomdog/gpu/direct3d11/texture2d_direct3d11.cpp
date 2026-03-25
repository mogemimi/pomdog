// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/direct3d11/texture2d_direct3d11.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/surface_format_helper.h"
#include "pomdog/gpu/backends/texture_helper.h"
#include "pomdog/gpu/dxgi/dxgi_format_helper.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/exception.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {

using Microsoft::WRL::ComPtr;

std::unique_ptr<Error>
Texture2DDirect3D11::initialize(
    unsafe_ptr<ID3D11Device> device,
    i32 pixelWidthIn,
    i32 pixelHeightIn,
    i32 levelCountIn,
    PixelFormat formatIn) noexcept
{
    pixelWidth_ = pixelWidthIn;
    pixelHeight_ = pixelHeightIn;
    levelCount_ = levelCountIn;
    format_ = formatIn;

    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(pixelWidth_ > 0);
    POMDOG_ASSERT(pixelHeight_ > 0);
    POMDOG_ASSERT(levelCount_ >= 1);

    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Format = dxgi::toDXGIFormat(format_);
    textureDesc.Width = pixelWidth_;
    textureDesc.Height = pixelHeight_;
    textureDesc.ArraySize = 1;
    textureDesc.MipLevels = levelCount_;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    if (auto hr = device->CreateTexture2D(&textureDesc, nullptr, &texture2D_); FAILED(hr)) {
        return errors::make("CreateTexture2D() failed: hr = " + std::to_string(hr));
    }

    // NOTE: Create the shader resource view (SRV)
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;

    if (auto hr = device->CreateShaderResourceView(texture2D_.Get(), &srvDesc, &shaderResourceView_); FAILED(hr)) {
        return errors::make("CreateShaderResourceView() failed: hr = " + std::to_string(hr));
    }

    return nullptr;
}

i32 Texture2DDirect3D11::getWidth() const noexcept
{
    return pixelWidth_;
}

i32 Texture2DDirect3D11::getHeight() const noexcept
{
    return pixelHeight_;
}

i32 Texture2DDirect3D11::getLevelCount() const noexcept
{
    return levelCount_;
}

PixelFormat
Texture2DDirect3D11::getFormat() const noexcept
{
    return format_;
}

void Texture2DDirect3D11::setData(const void* pixelData)
{
    POMDOG_ASSERT(texture2D_);
    POMDOG_ASSERT(pixelWidth_ > 0);
    POMDOG_ASSERT(pixelHeight_ > 0);
    POMDOG_ASSERT(levelCount_ >= 1);
    POMDOG_ASSERT(pixelData != nullptr);

    // NOTE: Get the device context
    ComPtr<ID3D11Device> device;
    texture2D_->GetDevice(&device);
    ComPtr<ID3D11DeviceContext> deviceContext;
    device->GetImmediateContext(&deviceContext);

    POMDOG_ASSERT(deviceContext != nullptr);

    auto bytesPerRow = static_cast<UINT>(pixelWidth_ * SurfaceFormatHelper::toBytesPerBlock(format_));

    switch (format_) {
    case PixelFormat::BlockComp1_UNorm:
        bytesPerRow = 8 * (std::max(pixelWidth_, 4) / 4);
        break;
    case PixelFormat::BlockComp2_UNorm:
    case PixelFormat::BlockComp3_UNorm:
        bytesPerRow = 16 * (std::max(pixelWidth_, 4) / 4);
        break;
    default:
        break;
    }

    deviceContext->UpdateSubresource(
        texture2D_.Get(),
        0,
        nullptr,
        pixelData,
        bytesPerRow,
        0);
}

void Texture2DDirect3D11::setData(
    i32 mipLevel,
    const Rect2D& region,
    const void* pixelData,
    u32 bytesPerRow)
{
    POMDOG_ASSERT(texture2D_);
    POMDOG_ASSERT(mipLevel >= 0);
    POMDOG_ASSERT(mipLevel < levelCount_);
    POMDOG_ASSERT(region.width > 0);
    POMDOG_ASSERT(region.height > 0);
    POMDOG_ASSERT(pixelData != nullptr);
    POMDOG_ASSERT(bytesPerRow > 0);

    // NOTE: Get the device context
    ComPtr<ID3D11Device> device;
    texture2D_->GetDevice(&device);
    ComPtr<ID3D11DeviceContext> deviceContext;
    device->GetImmediateContext(&deviceContext);

    POMDOG_ASSERT(deviceContext != nullptr);

    D3D11_BOX box;
    box.left = static_cast<UINT>(region.x);
    box.top = static_cast<UINT>(region.y);
    box.front = 0;
    box.right = static_cast<UINT>(region.x + region.width);
    box.bottom = static_cast<UINT>(region.y + region.height);
    box.back = 1;

    const auto subresource = D3D11CalcSubresource(
        static_cast<UINT>(mipLevel), 0, static_cast<UINT>(levelCount_));

    deviceContext->UpdateSubresource(
        texture2D_.Get(),
        subresource,
        &box,
        pixelData,
        static_cast<UINT>(bytesPerRow),
        0);
}

unsafe_ptr<ID3D11ShaderResourceView>
Texture2DDirect3D11::getShaderResourceView() const noexcept
{
    POMDOG_ASSERT(shaderResourceView_ != nullptr);
    return shaderResourceView_.Get();
}

} // namespace pomdog::gpu::detail::direct3d11
