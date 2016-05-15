// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Texture2DDirect3D11.hpp"
#include "../RenderSystem.DXGI/DXGIFormatHelper.hpp"
#include "../RenderSystem/TextureHelper.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <algorithm>

namespace Pomdog {
namespace Detail {
namespace Direct3D11 {

using DXGI::DXGIFormatHelper;
//-----------------------------------------------------------------------
Texture2DDirect3D11::Texture2DDirect3D11(
    ID3D11Device* device,
    const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContextIn,
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format)
    : deviceContext(deviceContextIn)
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
    //textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    //textureDesc.CPUAccessFlags = 0;
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
//-----------------------------------------------------------------------
void Texture2DDirect3D11::SetData(
    std::int32_t pixelWidth,
    std::int32_t pixelHeight,
    std::int32_t levelCount,
    SurfaceFormat format,
    const void* pixelData)
{
    POMDOG_ASSERT(texture2D);
    POMDOG_ASSERT(deviceContext);

    std::size_t sizeInBytes = TextureHelper::ComputeTextureSizeInBytes(
        pixelWidth, pixelHeight, levelCount, format);

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
//-----------------------------------------------------------------------
ID3D11ShaderResourceView* Texture2DDirect3D11::GetShaderResourceView() const
{
    POMDOG_ASSERT(shaderResourceView);
    return shaderResourceView.Get();
}
//-----------------------------------------------------------------------
} // namespace Direct3D11
} // namespace Detail
} // namespace Pomdog
