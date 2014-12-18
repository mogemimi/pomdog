//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Texture2DDirect3D11.hpp"
#include "DXGIFormatHelper.hpp"
#include <Pomdog/Graphics/SurfaceFormat.hpp>
#include <Pomdog/Utility/Exception.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <algorithm>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {
namespace {

static std::uint16_t ToBytesPerBlock(SurfaceFormat format)
{
	switch (format) {
	case SurfaceFormat::A8_UNorm:
	case SurfaceFormat::R8_UNorm:
	case SurfaceFormat::BlockComp1_UNorm:
		return 8;
	case SurfaceFormat::R8G8_UNorm:
	case SurfaceFormat::BlockComp2_UNorm:
	case SurfaceFormat::BlockComp3_UNorm:
		return 16;
	case SurfaceFormat::R8G8B8A8_UNorm:
	case SurfaceFormat::B8G8R8A8_UNorm:
	case SurfaceFormat::R10G10B10A2_UNorm:
	case SurfaceFormat::R11G11B10_Float:
	case SurfaceFormat::R16G16_Float:
	case SurfaceFormat::R32_Float:
		return 32;
	case SurfaceFormat::R32G32B32A32_Float:
		return 128;
	};

#ifdef _MSC_VER
	return 8;// FUS RO DAH!!!
#endif
}
//-----------------------------------------------------------------------
static std::size_t MipmapImageDataBytes(std::size_t pixelWidth, std::size_t pixelHeight, std::size_t bytesPerBlock)
{
	return ((pixelWidth + 3) / 4) * ((pixelHeight + 3) / 4) * bytesPerBlock;
}
//-----------------------------------------------------------------------
static std::size_t ComputeTextureBufferSize(std::size_t pixelWidth, std::size_t pixelHeight,
	std::uint32_t levelCount, SurfaceFormat format)
{
	POMDOG_ASSERT(pixelWidth > 0);
	POMDOG_ASSERT(pixelHeight > 0);
	POMDOG_ASSERT(levelCount >= 1);

	auto const bytesPerBlock = ToBytesPerBlock(format);

	std::size_t sizeInBytes = 0;
	std::size_t mipMapPixelWidth = pixelWidth;
	std::size_t mipMapPixelHeight = pixelHeight;

	for (std::size_t mipmapLevel = 0; mipmapLevel < levelCount; ++mipmapLevel)
	{
		sizeInBytes += MipmapImageDataBytes(pixelWidth, pixelHeight, bytesPerBlock);
		mipMapPixelWidth = std::max((mipMapPixelWidth >> 1), 1U);
		mipMapPixelHeight = std::max((mipMapPixelHeight >> 1), 1U);
	}

	return sizeInBytes;
}

}// unnamed namespace
//-----------------------------------------------------------------------
Texture2DDirect3D11::Texture2DDirect3D11(ID3D11Device* nativeDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContextIn,
	std::int32_t pixelWidth, std::int32_t pixelHeight,
	std::uint32_t levelCount, SurfaceFormat format)
	: deviceContext(deviceContextIn)
{
	POMDOG_ASSERT(pixelWidth > 0);
	POMDOG_ASSERT(pixelHeight > 0);

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
	textureDesc.BindFlags = (D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	auto hr = nativeDevice->CreateTexture2D(&textureDesc, nullptr, &nativeTexture2D);
	if (FAILED(hr))
	{
		//// FUS RO DAH!!!
		//POMDOG_THROW_EXCEPTION(ExceptionCode::RenderingAPIError, 
		//	"failed to create Direct3D11Texture.", "Texture2DDirect3D11::Texture2DDirect3D11");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;

	// Create the shader resource view.
	hr = nativeDevice->CreateShaderResourceView(nativeTexture2D.Get(),
		&shaderResourceViewDesc, &shaderResourceView);
	if (FAILED(hr))
	{
		//// FUS RO DAH!!!
		//POMDOG_THROW_EXCEPTION(ExceptionCode::RenderingAPIError, 
		//	"failed to create Direct3D11Texture.", "Texture2DDirect3D11::Texture2DDirect3D11");
	}
}
//-----------------------------------------------------------------------
void Texture2DDirect3D11::SetData(std::int32_t pixelWidth, std::int32_t pixelHeight,
	std::uint32_t levelCount, SurfaceFormat format, void const* pixelData)
{
	POMDOG_ASSERT(nativeTexture2D);
	POMDOG_ASSERT(deviceContext);

	std::size_t sizeInBytes = ComputeTextureBufferSize(pixelWidth, pixelHeight, levelCount, format);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	auto hr = deviceContext->Map(nativeTexture2D.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(hr))
	{
		///@todo Not implemented
		//// FUS RO DAH!!!
		//POMDOG_THROW_EXCEPTION(ExceptionCode::RenderingAPIError,
		//	"Failed to map buffer", "HardwareBufferHelper::SetData");
	}

	POMDOG_ASSERT(pixelData);
	POMDOG_ASSERT(sizeInBytes > 0);
	std::memcpy(mappedResource.pData, pixelData, sizeInBytes);

	deviceContext->Unmap(nativeTexture2D.Get(), 0);
}
//-----------------------------------------------------------------------
void Texture2DDirect3D11::Apply(std::uint32_t index)
{
	(&index);
	///@todo Not implemented
	POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
ID3D11ShaderResourceView* Texture2DDirect3D11::ShaderResourceView() const
{
	POMDOG_ASSERT(shaderResourceView);
	return shaderResourceView.Get();
}
//-----------------------------------------------------------------------
}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
