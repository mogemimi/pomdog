// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Texture2DDirect3D11.hpp"
#include "../RenderSystem.DXGI/DXGIFormatHelper.hpp"
#include "../RenderSystem/TextureHelper.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <algorithm>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {

using DXGI::DXGIFormatHelper;
//-----------------------------------------------------------------------
Texture2DDirect3D11::Texture2DDirect3D11(ID3D11Device* nativeDevice,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContextIn,
    std::int32_t pixelWidth, std::int32_t pixelHeight,
    std::int32_t levelCount, SurfaceFormat format)
    : deviceContext(deviceContextIn)
{
    POMDOG_ASSERT(nativeDevice != nullptr);
    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);
    POMDOG_ASSERT(levelCount >= 1);

    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.Width = pixelWidth;
    textureDesc.Height = pixelHeight;
    textureDesc.MipLevels = levelCount;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGIFormatHelper::ToDXGIFormat(format);
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DYNAMIC;
    //textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    //textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    auto hr = nativeDevice->CreateTexture2D(&textureDesc, nullptr, &texture2D);
    if (FAILED(hr))
    {
        ///@error FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create D3D11Texture2D");
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;

    // Create the shader resource view.
    hr = nativeDevice->CreateShaderResourceView(texture2D.Get(),
        &shaderResourceViewDesc, &shaderResourceView);
    if (FAILED(hr))
    {
        ///@error FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create the shader resource view");
    }
}
//-----------------------------------------------------------------------
void Texture2DDirect3D11::SetData(std::int32_t pixelWidth, std::int32_t pixelHeight,
    std::int32_t levelCount, SurfaceFormat format, void const* pixelData)
{
    POMDOG_ASSERT(texture2D);
    POMDOG_ASSERT(deviceContext);

    std::size_t sizeInBytes = TextureHelper::ComputeTextureSizeInBytes(
        pixelWidth, pixelHeight, levelCount, format);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto hr = deviceContext->Map(texture2D.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(hr))
    {
        ///@error FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to map buffer");
    }

    POMDOG_ASSERT(pixelData);
    POMDOG_ASSERT(sizeInBytes > 0);
    std::memcpy(mappedResource.pData, pixelData, sizeInBytes);

    deviceContext->Unmap(texture2D.Get(), 0);
}
//-----------------------------------------------------------------------
ID3D11ShaderResourceView* Texture2DDirect3D11::ShaderResourceView() const
{
    POMDOG_ASSERT(shaderResourceView);
    return shaderResourceView.Get();
}
//-----------------------------------------------------------------------
} // namespace Direct3D11
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog
