//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "RenderTarget2DDirect3D11.hpp"
#include "DXGIFormatHelper.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {
namespace {

using Microsoft::WRL::ComPtr;

static void BuildRenderTarget(ID3D11Device* nativeDevice, SurfaceFormat format,
	std::int32_t pixelWidth, std::int32_t pixelHeight, std::uint32_t levelCount, bool isSharedTexture,
	ComPtr<ID3D11Texture2D> & renderTexture,
	ComPtr<ID3D11RenderTargetView> & renderTargetView,
	ComPtr<ID3D11ShaderResourceView> & textureResourceView)
{
	POMDOG_ASSERT(pixelWidth > 0);
	POMDOG_ASSERT(pixelHeight > 0);

	// Create a render texture
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = pixelWidth;
	textureDesc.Height = pixelHeight;
	textureDesc.MipLevels = levelCount;
	textureDesc.ArraySize = 1;
	textureDesc.Format = (isSharedTexture ? DXGI_FORMAT_B8G8R8A8_UNORM : DXGIFormatHelper::ToDXGIFormat(format));
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = (isSharedTexture ? D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX : 0);

	POMDOG_ASSERT(nativeDevice);
	HRESULT	hr = nativeDevice->CreateTexture2D(&textureDesc, nullptr, &renderTexture);

	if (FAILED(hr)) {
		// FUS RO DAH!
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create render target texture");
	}

	// Create a render target view
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	POMDOG_ASSERT(renderTexture);
	hr = nativeDevice->CreateRenderTargetView(renderTexture.Get(), &renderTargetViewDesc, &renderTargetView);

	if (FAILED(hr)) {
		// FUS RO DAH!
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create RendetTargetView");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;

	// Create shader resource view
	hr = nativeDevice->CreateShaderResourceView(renderTexture.Get(), &shaderResourceViewDesc, &textureResourceView);
	if (FAILED(hr)) {
		// FUS RO DAH!
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create shader resource view");
	}
}
//-----------------------------------------------------------------------
static void BuildDepthBuffer(ID3D11Device* nativeDevice, DepthFormat depthStencilFormat,
	std::int32_t pixelWidth, std::int32_t pixelHeight, std::uint32_t levelCount,
	ComPtr<ID3D11Texture2D> & depthStencil,
	ComPtr<ID3D11DepthStencilView> & depthStencilView)
{
	if (DepthFormat::None == depthStencilFormat) {
		return;
	}

	POMDOG_ASSERT(pixelWidth > 0);
	POMDOG_ASSERT(pixelHeight > 0);

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = pixelWidth;
	descDepth.Height = pixelHeight;
	descDepth.MipLevels = levelCount;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGIFormatHelper::ToDXGIFormat(depthStencilFormat);
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	POMDOG_ASSERT(nativeDevice);
	HRESULT hr = nativeDevice->CreateTexture2D(&descDepth, nullptr, &depthStencil);

	if (FAILED(hr)) {
		// FUS RO DAH!
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create depth stencil buffer");
	}

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	POMDOG_ASSERT(depthStencil);
	hr = nativeDevice->CreateDepthStencilView(depthStencil.Get(), &descDSV, &depthStencilView);

	if (FAILED(hr)) {
		// FUS RO DAH!
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create DepthStencilView");
	}
}
//-----------------------------------------------------------------------
static void BuildBackBufferBySwapChain(ID3D11Device* nativeDevice, IDXGISwapChain* swapChain,
	ComPtr<ID3D11Texture2D> & renderTexture,
	ComPtr<ID3D11RenderTargetView> & renderTargetView)
{
	POMDOG_ASSERT(nativeDevice);
	POMDOG_ASSERT(swapChain);

	// Get a surface in the swap chain
	HRESULT	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(renderTexture.GetAddressOf()));

	if (FAILED(hr)) {
		// FUS RO DAH!
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to call IDXGISwapChain::GetBuffer");
	}

	// Create a render target view
	POMDOG_ASSERT(renderTexture);
	hr = nativeDevice->CreateRenderTargetView(renderTexture.Get(), nullptr, &renderTargetView);

	if (FAILED(hr)) {
		// FUS RO DAH!
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create RenderTargetView");
	}
}

}// unnamed namespace
//-----------------------------------------------------------------------
RenderTarget2DDirect3D11::RenderTarget2DDirect3D11(ID3D11Device* nativeDevice,
	std::int32_t pixelWidth, std::int32_t pixelHeight,
	std::uint32_t levelCount, SurfaceFormat format, DepthFormat depthStencilFormat)
{
	BuildRenderTarget(nativeDevice, format, pixelWidth, pixelHeight, levelCount,
		false, renderTexture, renderTargetView, textureResourceView);

	BuildDepthBuffer(nativeDevice, depthStencilFormat, pixelWidth, pixelHeight, levelCount,
		depthStencil, depthStencilView);
}
//-----------------------------------------------------------------------
RenderTarget2DDirect3D11::RenderTarget2DDirect3D11(ID3D11Device* nativeDevice,
	IDXGISwapChain* swapChain, std::int32_t pixelWidth, std::int32_t pixelHeight,
	DepthFormat depthStencilFormat)
{
	constexpr std::uint32_t backBufferMipLevels = 1;

	BuildBackBufferBySwapChain(nativeDevice, swapChain,
		renderTexture, renderTargetView);

	BuildDepthBuffer(nativeDevice, depthStencilFormat, pixelWidth, pixelHeight, backBufferMipLevels,
		depthStencil, depthStencilView);
}
//-----------------------------------------------------------------------
void RenderTarget2DDirect3D11::Apply(std::uint32_t index)
{
	(&index);

	///@todo Not implemented
	POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
ID3D11RenderTargetView* RenderTarget2DDirect3D11::RenderTargetView() const
{
	POMDOG_ASSERT(renderTargetView);
	return renderTargetView.Get();
}
//-----------------------------------------------------------------------
ID3D11DepthStencilView* RenderTarget2DDirect3D11::DepthStencilView() const
{
	return depthStencilView.Get();
}
//-----------------------------------------------------------------------
ID3D11ShaderResourceView* RenderTarget2DDirect3D11::ShaderResourceView() const
{
	POMDOG_ASSERT(textureResourceView);
	return textureResourceView.Get();
}
//-----------------------------------------------------------------------
void RenderTarget2DDirect3D11::ResetBackBuffer(ID3D11Device* nativeDevice, IDXGISwapChain* swapChain,
	std::int32_t pixelWidth, std::int32_t pixelHeight, DepthFormat depthStencilFormat)
{
	POMDOG_ASSERT(nativeDevice != nullptr);
	POMDOG_ASSERT(swapChain != nullptr);

	renderTargetView.Reset();
	renderTexture.Reset();
	depthStencilView.Reset();
	depthStencil.Reset();

	constexpr std::uint32_t backBufferMipLevels = 1;

	BuildBackBufferBySwapChain(nativeDevice, swapChain,
		renderTexture, renderTargetView);

	BuildDepthBuffer(nativeDevice, depthStencilFormat, pixelWidth, pixelHeight, backBufferMipLevels,
		depthStencil, depthStencilView);
}
//-----------------------------------------------------------------------
void RenderTarget2DDirect3D11::ResetBackBuffer()
{
	renderTargetView.Reset();
	renderTexture.Reset();
}
//-----------------------------------------------------------------------
}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog
