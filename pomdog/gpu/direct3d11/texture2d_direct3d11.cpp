// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/direct3d11/texture2d_direct3d11.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/surface_format_helper.h"
#include "pomdog/gpu/backends/texture_helper.h"
#include "pomdog/gpu/dxgi/dxgi_format_helper.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/exception.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {
namespace {

void loadPixelData(
    void* output,
    const void* input,
    std::int32_t width,
    std::int32_t height,
    std::int32_t levelCount,
    PixelFormat format)
{
    std::size_t sizeInBytes = TextureHelper::computeTextureSizeInBytes(
        width, height, levelCount, format);

    POMDOG_ASSERT(input != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    std::memcpy(output, input, sizeInBytes);
}

void loadPixelDataWithStride(
    void* output,
    const void* input,
    std::int32_t height,
    std::size_t outputRowPitch,
    std::size_t inputRowPitch)
{
    for (int y = 0; y < height; y++) {
        auto dst = reinterpret_cast<std::uint8_t*>(output) + y * outputRowPitch;
        auto src = reinterpret_cast<const std::uint8_t*>(input) + y * inputRowPitch;
        std::memcpy(dst, src, inputRowPitch);
    }
}

void loadPixelDataBlockComp1(
    void* output,
    const void* input,
    std::int32_t width,
    std::int32_t height,
    std::size_t outputRowPitch)
{
    const auto bytesPerRow = static_cast<std::size_t>(8 * (std::max(width, 4) / 4));

    for (int y = 0; y < height / 4; y++) {
        auto dst = reinterpret_cast<std::uint8_t*>(output) + y * outputRowPitch;
        auto src = reinterpret_cast<const std::uint8_t*>(input) + y * bytesPerRow;
        for (int x = 0; x < width / 4; x++) {
            std::memcpy(dst, src, bytesPerRow);
        }
    }
}

} // namespace

using Microsoft::WRL::ComPtr;

std::unique_ptr<Error>
Texture2DDirect3D11::initialize(
    unsafe_ptr<ID3D11Device> device,
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    std::int32_t levelCountIn,
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
    textureDesc.Usage = D3D11_USAGE_DYNAMIC;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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

std::int32_t
Texture2DDirect3D11::getWidth() const noexcept
{
    return pixelWidth_;
}

std::int32_t
Texture2DDirect3D11::getHeight() const noexcept
{
    return pixelHeight_;
}

std::int32_t
Texture2DDirect3D11::getLevelCount() const noexcept
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

    // NOTE: Map the texture
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto hr = deviceContext->Map(
        texture2D_.Get(),
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mappedResource);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to map buffer");
    }

    const auto inputRowPitch = static_cast<UINT>(pixelWidth_ * SurfaceFormatHelper::toBytesPerBlock(format_));
    if (inputRowPitch == mappedResource.RowPitch) {
        loadPixelData(mappedResource.pData, pixelData, pixelWidth_, pixelHeight_, levelCount_, format_);
    }
    else {
        switch (format_) {
        case PixelFormat::R8_UNorm:
        case PixelFormat::R8G8_UNorm:
        case PixelFormat::R8G8B8A8_UNorm:
            loadPixelDataWithStride(mappedResource.pData, pixelData, pixelHeight_, mappedResource.RowPitch, inputRowPitch);
            break;
        case PixelFormat::BlockComp1_UNorm:
            loadPixelDataBlockComp1(mappedResource.pData, pixelData, pixelWidth_, pixelHeight_, mappedResource.RowPitch);
            break;
        case PixelFormat::A8_UNorm:
        case PixelFormat::B8G8R8A8_UNorm:
        case PixelFormat::R10G10B10A2_UNorm:
        case PixelFormat::R11G11B10_Float:
        case PixelFormat::R16G16_Float:
        case PixelFormat::R16G16B16A16_Float:
        case PixelFormat::R32_Float:
        case PixelFormat::R32G32B32A32_Float:
        case PixelFormat::BlockComp2_UNorm:
        case PixelFormat::BlockComp3_UNorm:
            loadPixelData(mappedResource.pData, pixelData, pixelWidth_, pixelHeight_, levelCount_, format_);
            break;
        default:
            // FIXME: Not implemented yet.
            break;
        }
    }

    deviceContext->Unmap(texture2D_.Get(), 0);
}

unsafe_ptr<ID3D11ShaderResourceView>
Texture2DDirect3D11::getShaderResourceView() const noexcept
{
    POMDOG_ASSERT(shaderResourceView_ != nullptr);
    return shaderResourceView_.Get();
}

} // namespace pomdog::gpu::detail::direct3d11
