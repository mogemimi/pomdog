// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Texture2DDirect3D11.hpp"
#include "../RenderSystem.DXGI/DXGIFormatHelper.hpp"
#include "../RenderSystem/SurfaceFormatHelper.hpp"
#include "../RenderSystem/TextureHelper.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <algorithm>

namespace Pomdog::Detail::Direct3D11 {
namespace {

void LoadPixelData(
    void* output,
    const void* input,
    std::int32_t width,
    std::int32_t height,
    std::int32_t levelCount,
    SurfaceFormat format)
{
    std::size_t sizeInBytes = TextureHelper::ComputeTextureSizeInBytes(
        width, height, levelCount, format);

    POMDOG_ASSERT(input != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    std::memcpy(output, input, sizeInBytes);
}

void LoadPixelDataWithStride(
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

void LoadPixelDataBlockComp1(
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

using DXGI::DXGIFormatHelper;
using Microsoft::WRL::ComPtr;

Texture2DDirect3D11::Texture2DDirect3D11(
    ID3D11Device* device,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format)
{
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);

    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Format = DXGIFormatHelper::ToDXGIFormat(format);
    textureDesc.Width = pixelWidth;
    textureDesc.Height = pixelHeight;
    textureDesc.ArraySize = 1;
    textureDesc.MipLevels = levelCount;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DYNAMIC;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    textureDesc.MiscFlags = 0;

    auto hr = device->CreateTexture2D(&textureDesc, nullptr, &texture2D);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create D3D11Texture2D");
    }

    // Create the shader resource view (SRV)
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;

    hr = device->CreateShaderResourceView(
        texture2D.Get(), &srvDesc, &shaderResourceView);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create the shader resource view");
    }
}

void Texture2DDirect3D11::SetData(
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format,
    const void* pixelData)
{
    POMDOG_ASSERT(texture2D);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);

    // NOTE: Get the device context
    ComPtr<ID3D11Device> device;
    texture2D->GetDevice(&device);
    ComPtr<ID3D11DeviceContext> deviceContext;
    device->GetImmediateContext(&deviceContext);

    POMDOG_ASSERT(deviceContext != nullptr);

    // NOTE: Map the texture
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto hr = deviceContext->Map(texture2D.Get(), 0,
        D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to map buffer");
    }

    const auto inputRowPitch = static_cast<UINT>(pixelWidth * SurfaceFormatHelper::ToBytesPerBlock(format));
    if (inputRowPitch == mappedResource.RowPitch) {
        LoadPixelData(mappedResource.pData, pixelData, pixelWidth, pixelHeight, levelCount, format);
    }
    else {
        switch (format) {
        case SurfaceFormat::R8_UNorm:
        case SurfaceFormat::R8G8_UNorm:
        case SurfaceFormat::R8G8B8A8_UNorm:
            LoadPixelDataWithStride(mappedResource.pData, pixelData, pixelHeight, mappedResource.RowPitch, inputRowPitch);
            break;
        case SurfaceFormat::BlockComp1_UNorm:
            LoadPixelDataBlockComp1(mappedResource.pData, pixelData, pixelWidth, pixelHeight, mappedResource.RowPitch);
            break;
        case SurfaceFormat::A8_UNorm:
        case SurfaceFormat::B8G8R8A8_UNorm:
        case SurfaceFormat::R10G10B10A2_UNorm:
        case SurfaceFormat::R11G11B10_Float:
        case SurfaceFormat::R16G16_Float:
        case SurfaceFormat::R16G16B16A16_Float:
        case SurfaceFormat::R32_Float:
        case SurfaceFormat::R32G32B32A32_Float:
        case SurfaceFormat::BlockComp2_UNorm:
        case SurfaceFormat::BlockComp3_UNorm:
            LoadPixelData(mappedResource.pData, pixelData, pixelWidth, pixelHeight, levelCount, format);
            break;
        }
    }

    deviceContext->Unmap(texture2D.Get(), 0);
}

ID3D11ShaderResourceView* Texture2DDirect3D11::GetShaderResourceView() const
{
    POMDOG_ASSERT(shaderResourceView);
    return shaderResourceView.Get();
}

} // namespace Pomdog::Detail::Direct3D11
