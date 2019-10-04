// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Texture2DDirect3D11.hpp"
#include "../RenderSystem.DXGI/DXGIFormatHelper.hpp"
#include "../RenderSystem/TextureHelper.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <algorithm>

namespace Pomdog::Detail::Direct3D11 {

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

    // NOTE: Get the device context
    ComPtr<ID3D11Device> device;
    texture2D->GetDevice(&device);
    ComPtr<ID3D11DeviceContext> deviceContext;
    device->GetImmediateContext(&deviceContext);

    POMDOG_ASSERT(deviceContext != nullptr);

    std::size_t sizeInBytes = TextureHelper::ComputeTextureSizeInBytes(
        pixelWidth, pixelHeight, levelCount, format);

    // NOTE: Map the texture
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto hr = deviceContext->Map(texture2D.Get(), 0,
        D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to map buffer");
    }

    POMDOG_ASSERT(pixelData);
    POMDOG_ASSERT(sizeInBytes > 0);
    std::memcpy(mappedResource.pData, pixelData, sizeInBytes);

    deviceContext->Unmap(texture2D.Get(), 0);
}

ID3D11ShaderResourceView* Texture2DDirect3D11::GetShaderResourceView() const
{
    POMDOG_ASSERT(shaderResourceView);
    return shaderResourceView.Get();
}

} // namespace Pomdog::Detail::Direct3D11
