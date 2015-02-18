//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "GraphicsContextDirect3D11.hpp"
#include "ConstantLayoutDirect3D11.hpp"
#include "DXGIFormatHelper.hpp"
#include "EffectPassDirect3D11.hpp"
#include "IndexBufferDirect3D11.hpp"
#include "InputLayoutDirect3D11.hpp"
#include "VertexBufferDirect3D11.hpp"
#include "Texture2DDirect3D11.hpp"
#include "RenderTarget2DDirect3D11.hpp"
#include "../RenderSystem/GraphicsCapabilities.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Graphics/ClearOptions.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/StringFormat.hpp"
#include <algorithm>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {
namespace {

static D3D11_PRIMITIVE_TOPOLOGY ToD3D11PrimitiveTopology(PrimitiveTopology primitiveTopology)
{
	switch (primitiveTopology) {
	case PrimitiveTopology::TriangleStrip: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	case PrimitiveTopology::TriangleList: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case PrimitiveTopology::LineList: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	case PrimitiveTopology::LineStrip: return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
	}
	return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
}
//-----------------------------------------------------------------------
static DXGI_FORMAT ToDXGIFormat(IndexElementSize elementSize)
{
	switch (elementSize) {
	case IndexElementSize::SixteenBits: return DXGI_FORMAT_R16_UINT;
	case IndexElementSize::ThirtyTwoBits: return DXGI_FORMAT_R32_UINT;
	}
	return DXGI_FORMAT_UNKNOWN;
}

}// unnamed namespace
//-----------------------------------------------------------------------
GraphicsContextDirect3D11::GraphicsContextDirect3D11(
	HWND windowHandle,
	Microsoft::WRL::ComPtr<IDXGIFactory1> const& dxgiFactory,
	Microsoft::WRL::ComPtr<ID3D11Device> const& nativeDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContextIn)
	: deviceContext(deviceContextIn)
	, preferredBackBufferWidth(1)
	, preferredBackBufferHeight(1)
	, backBufferCount(2)
	, backBufferFormat(SurfaceFormat::R8G8B8A8_UNorm)
	, backBufferDepthFormat(DepthFormat::Depth24Stencil8)
{
	using Microsoft::WRL::ComPtr;

	POMDOG_ASSERT(deviceContext);

	DXGI_SAMPLE_DESC sampleDesc;
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;
	{
		//UINT msaaSampleCount = 4;//x1, x2, x4, x8, x16 ...
		//
		//if (msaaSampleCount > D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT) {
		//	msaaSampleCount = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT;
		//}
		//
		//for (UINT i = 0; i <= msaaSampleCount; ++i)
		//{
		//	UINT numQualityLevels(0);
		//
		//	auto hr = nativeDevice->CheckMultisampleQualityLevels(
		//		DXGI_FORMAT_R8G8B8A8_UNORM, i, &numQualityLevels );
		//
		//	if (SUCCEEDED(hr)) {
		//		if (0 < numQualityLevels) {
		//			sampleDesc.Count = i;
		//			sampleDesc.Quality = numQualityLevels - 1;
		//		}
		//	}
		//}

#if defined(DEBUG) && !defined(NDEBUG)
		{
			Log::Internal(StringFormat(
				"MSAA.Sample.Count  : %d\n"
				"MSAA.Sample.Quality: %d",
				sampleDesc.Count,
				sampleDesc.Quality));
		}
#endif
	}
	{
		RECT rect;
		::GetClientRect(windowHandle, &rect);
		auto const windowWidth = rect.right - rect.left;
		auto const windowHeight = rect.bottom - rect.top;

		preferredBackBufferWidth = std::max<int>(preferredBackBufferWidth, windowWidth);
		preferredBackBufferHeight = std::max<int>(preferredBackBufferHeight, windowHeight);
	}
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = backBufferCount;
		swapChainDesc.BufferDesc.Width = preferredBackBufferWidth;
		swapChainDesc.BufferDesc.Height = preferredBackBufferHeight;
		swapChainDesc.BufferDesc.Format = DXGIFormatHelper::ToDXGIFormat(backBufferFormat);
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = windowHandle;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SampleDesc.Count = sampleDesc.Count;
		swapChainDesc.SampleDesc.Quality = sampleDesc.Quality;

		POMDOG_ASSERT(nativeDevice);
		POMDOG_ASSERT(dxgiFactory);
		HRESULT hr = dxgiFactory->CreateSwapChain(nativeDevice.Get(), &swapChainDesc, &swapChain);

		if (FAILED(hr)) {
			// FUS RO DAH!
			POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create SwapChain");
		}
	}
	{
		backBuffer = std::make_shared<RenderTarget2DDirect3D11>(nativeDevice.Get(),
			swapChain.Get(), preferredBackBufferWidth, preferredBackBufferHeight,
			backBufferDepthFormat);

		boundRenderTargets.reserve(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
		boundRenderTargets.push_back(backBuffer);
	}

	SetRenderTarget();
}
//-----------------------------------------------------------------------
GraphicsContextDirect3D11::~GraphicsContextDirect3D11()
{
	boundRenderTargets.clear();
	backBuffer.reset();
	swapChain.Reset();
	deviceContext.Reset();
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::Clear(Color const& color)
{
	auto const fillColor = color.ToVector4();

	for (auto & renderTarget: boundRenderTargets)
	{
		POMDOG_ASSERT(renderTarget);
		deviceContext->ClearRenderTargetView(renderTarget->RenderTargetView(), fillColor.Data());

		if (auto depthStencilView = renderTarget->DepthStencilView())
		{
			deviceContext->ClearDepthStencilView(depthStencilView,
				D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
	}
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::Clear(ClearOptions options, Color const& color, float depth, std::int32_t stencil)
{
	POMDOG_ASSERT(stencil <= std::numeric_limits<UINT8>::max());
	POMDOG_ASSERT(stencil >= 0);

	auto const fillColor = color.ToVector4();

	UINT mask = 0;

	if ((options | ClearOptions::DepthBuffer) == options) {
		mask |= D3D11_CLEAR_DEPTH;
	}
	if ((options | ClearOptions::Stencil) == options) {
		mask |= D3D11_CLEAR_STENCIL;
	}

	for (auto & renderTarget : boundRenderTargets)
	{
		POMDOG_ASSERT(renderTarget);

		if ((options | ClearOptions::RenderTarget) == options) {
			deviceContext->ClearRenderTargetView(renderTarget->RenderTargetView(), fillColor.Data());
		}

		if (auto depthStencilView = renderTarget->DepthStencilView())
		{
			deviceContext->ClearDepthStencilView(depthStencilView,
				mask, depth, static_cast<UINT8>(stencil));
		}
	}
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::Present()
{
	POMDOG_ASSERT(swapChain);
	swapChain->Present(0, 0);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::Draw(PrimitiveTopology primitiveTopology, std::uint32_t vertexCount)
{
	POMDOG_ASSERT(deviceContext);

	deviceContext->IASetPrimitiveTopology(ToD3D11PrimitiveTopology(primitiveTopology));

	deviceContext->Draw(vertexCount, 0);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::DrawIndexed(PrimitiveTopology primitiveTopology,
	std::shared_ptr<IndexBuffer> const& indexBuffer, std::uint32_t indexCount)
{
	POMDOG_ASSERT(deviceContext);

	POMDOG_ASSERT(indexBuffer);
	POMDOG_ASSERT(indexBuffer->NativeIndexBuffer());
	POMDOG_ASSERT(dynamic_cast<IndexBufferDirect3D11*>(indexBuffer->NativeIndexBuffer()));

	auto const nativeIndexBuffer = static_cast<IndexBufferDirect3D11*>(indexBuffer->NativeIndexBuffer());

	deviceContext->IASetIndexBuffer(nativeIndexBuffer->NativeBuffer(),
		ToDXGIFormat(indexBuffer->ElementSize()), 0);

	deviceContext->IASetPrimitiveTopology(ToD3D11PrimitiveTopology(primitiveTopology));

	deviceContext->DrawIndexed(indexCount, 0, 0);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::DrawInstanced(PrimitiveTopology primitiveTopology, std::uint32_t vertexCount, std::uint32_t instanceCount)
{
	POMDOG_ASSERT(deviceContext);

	deviceContext->IASetPrimitiveTopology(ToD3D11PrimitiveTopology(primitiveTopology));

	deviceContext->DrawInstanced(vertexCount, instanceCount, 0, 0);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::DrawIndexedInstanced(PrimitiveTopology primitiveTopology,
	std::shared_ptr<IndexBuffer> const& indexBuffer, std::uint32_t indexCount, std::uint32_t instanceCount)
{
	POMDOG_ASSERT(deviceContext);

	POMDOG_ASSERT(indexBuffer);
	POMDOG_ASSERT(indexBuffer->NativeIndexBuffer());
	POMDOG_ASSERT(dynamic_cast<IndexBufferDirect3D11*>(indexBuffer->NativeIndexBuffer()));

	auto const nativeIndexBuffer = static_cast<IndexBufferDirect3D11*>(indexBuffer->NativeIndexBuffer());

	deviceContext->IASetIndexBuffer(nativeIndexBuffer->NativeBuffer(),
		ToDXGIFormat(indexBuffer->ElementSize()), 0);

	deviceContext->IASetPrimitiveTopology(ToD3D11PrimitiveTopology(primitiveTopology));

	deviceContext->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);
}
//-----------------------------------------------------------------------
GraphicsCapabilities GraphicsContextDirect3D11::GetCapabilities() const
{
	GraphicsCapabilities caps;
	caps.SamplerSlotCount = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT;
	return std::move(caps);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetViewport(Viewport const& viewportIn)
{
	POMDOG_ASSERT(0.0f <= viewportIn.Width());
	POMDOG_ASSERT(0.0f <= viewportIn.Height());
	POMDOG_ASSERT(D3D11_VIEWPORT_BOUNDS_MAX >= viewportIn.TopLeftX() + viewportIn.Width());
	POMDOG_ASSERT(D3D11_VIEWPORT_BOUNDS_MAX >= viewportIn.TopLeftY() + viewportIn.Height());

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<FLOAT>(viewportIn.Width());
	viewport.Height = static_cast<FLOAT>(viewportIn.Height());
	viewport.MinDepth = viewportIn.MinDepth;
	viewport.MaxDepth = viewportIn.MaxDepth;
	viewport.TopLeftX = static_cast<FLOAT>(viewportIn.TopLeftX());
	viewport.TopLeftY = static_cast<FLOAT>(viewportIn.TopLeftY());

	POMDOG_ASSERT(deviceContext);
	deviceContext->RSSetViewports(1, &viewport);
}
//-----------------------------------------------------------------------
Rectangle GraphicsContextDirect3D11::GetScissorRectangle() const
{
	///@todo Not implemented
	POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetScissorRectangle(Rectangle const& rectangle)
{
	POMDOG_ASSERT(deviceContext);

	std::vector<D3D11_RECT> rects;
	rects.resize(std::max<std::size_t>(1, boundRenderTargets.size()));

	D3D11_RECT rect;
	rect.bottom = rectangle.Bottom();
	rect.left = rectangle.Left();
	rect.right = rectangle.Right();
	rect.top = rectangle.Top();

	std::fill(std::begin(rects), std::end(rects), rect);

	POMDOG_ASSERT(!rects.empty());

	deviceContext->RSSetScissorRects(rects.size(), rects.data());
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffersIn)
{
	POMDOG_ASSERT(!vertexBuffersIn.empty());

	std::vector<UINT> strides;
	strides.reserve(vertexBuffersIn.size());

	std::vector<UINT> offsets;
	offsets.reserve(vertexBuffersIn.size());

	std::vector<ID3D11Buffer*> vertexBuffers;
	vertexBuffers.reserve(vertexBuffersIn.size());

	for (auto & vertexBuffer: vertexBuffersIn)
	{
		POMDOG_ASSERT(vertexBuffer);
		POMDOG_ASSERT(vertexBuffer->NativeVertexBuffer());

		auto nativeVertexBuffer = static_cast<VertexBufferDirect3D11*>(vertexBuffer->NativeVertexBuffer());

		POMDOG_ASSERT(nativeVertexBuffer);
		POMDOG_ASSERT(nativeVertexBuffer == dynamic_cast<VertexBufferDirect3D11*>(vertexBuffer->NativeVertexBuffer()));

		vertexBuffers.push_back(nativeVertexBuffer->NativeBuffer());
		strides.push_back(vertexBuffer->StrideBytes());
		offsets.push_back(0);
	}

	POMDOG_ASSERT(deviceContext);
	deviceContext->IASetVertexBuffers(0, vertexBuffers.size(), vertexBuffers.data(),
		strides.data(), offsets.data());
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetTexture(std::uint32_t index)
{
	POMDOG_ASSERT(index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	POMDOG_ASSERT(index < boundTextureViews.size());

	boundTextureViews[index] = nullptr;

	POMDOG_ASSERT(deviceContext);

	deviceContext->PSSetShaderResources(index, 1, &boundTextureViews[index]);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetTexture(std::uint32_t index, Texture2D & textureIn)
{
	POMDOG_ASSERT(index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	POMDOG_ASSERT(index < boundTextureViews.size());

	POMDOG_ASSERT(textureIn.NativeTexture2D());
	POMDOG_ASSERT(dynamic_cast<Texture2DDirect3D11*>(textureIn.NativeTexture2D()));
	auto nativeTexture = static_cast<Texture2DDirect3D11*>(textureIn.NativeTexture2D());

	boundTextureViews[index] = nativeTexture->ShaderResourceView();

	POMDOG_ASSERT(deviceContext);
	deviceContext->PSSetShaderResources(0, 1, &boundTextureViews[index]);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetTexture(std::uint32_t index, RenderTarget2D & textureIn)
{
	POMDOG_ASSERT(index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	POMDOG_ASSERT(index < boundTextureViews.size());

	POMDOG_ASSERT(textureIn.NativeRenderTarget2D());
	POMDOG_ASSERT(dynamic_cast<RenderTarget2DDirect3D11*>(textureIn.NativeRenderTarget2D()));
	auto nativeTexture = static_cast<RenderTarget2DDirect3D11*>(textureIn.NativeRenderTarget2D());

	boundTextureViews[index] = nativeTexture->ShaderResourceView();

	POMDOG_ASSERT(deviceContext);
	deviceContext->PSSetShaderResources(0, 1, &boundTextureViews[index]);
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetRenderTarget()
{
	boundRenderTargets.clear();
	boundRenderTargets.push_back(backBuffer);

	std::array<ID3D11RenderTargetView*, 1> renderTargetViews = { backBuffer->RenderTargetView() };

	deviceContext->OMSetRenderTargets(renderTargetViews.size(), renderTargetViews.data(),
		boundRenderTargets.front()->DepthStencilView());
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets)
{
	POMDOG_ASSERT(!renderTargets.empty());
	POMDOG_ASSERT(renderTargets.size() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

	boundRenderTargets.clear();

	std::array<ID3D11RenderTargetView*, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> renderTargetViews;
	std::fill(std::begin(renderTargetViews), std::end(renderTargetViews), nullptr);

	for (std::size_t i = 0; i < renderTargets.size(); ++i)
	{
		POMDOG_ASSERT(renderTargets[i]);
		POMDOG_ASSERT(renderTargets[i]->NativeRenderTarget2D());
		POMDOG_ASSERT(dynamic_cast<RenderTarget2DDirect3D11*>(renderTargets[i]->NativeRenderTarget2D()));

		auto nativeRenderTarget = static_cast<RenderTarget2DDirect3D11*>(renderTargets[i]->NativeRenderTarget2D());
		boundRenderTargets.emplace_back(renderTargets[i], nativeRenderTarget);

		renderTargetViews[i] = nativeRenderTarget->RenderTargetView();
	}

	POMDOG_ASSERT(renderTargetViews.size() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

	deviceContext->OMSetRenderTargets(renderTargetViews.size(), renderTargetViews.data(),
		boundRenderTargets.front()->DepthStencilView());
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetEffectPass(std::shared_ptr<NativeEffectPass> const& nativeEffectPass)
{
	POMDOG_ASSERT(deviceContext);

	POMDOG_ASSERT(nativeEffectPass);
	POMDOG_ASSERT(dynamic_cast<EffectPassDirect3D11*>(nativeEffectPass.get()));

	auto & effectPass = static_cast<EffectPassDirect3D11&>(*nativeEffectPass);
	effectPass.Apply(deviceContext.Get());
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::SetConstantBuffers(std::shared_ptr<NativeConstantLayout> const& nativeConstantLayout)
{
	POMDOG_ASSERT(deviceContext);

	POMDOG_ASSERT(nativeConstantLayout);
	POMDOG_ASSERT(dynamic_cast<ConstantLayoutDirect3D11*>(nativeConstantLayout.get()));

	auto & constantLayout = static_cast<ConstantLayoutDirect3D11&>(*nativeConstantLayout);
	constantLayout.Apply(deviceContext.Get());
}
//-----------------------------------------------------------------------
void GraphicsContextDirect3D11::ResizeBackBuffers(ID3D11Device* nativeDevice,
	int backBufferWidthIn, int backBufferHeightIn)
{
	POMDOG_ASSERT(nativeDevice != nullptr);
	POMDOG_ASSERT(backBufferWidthIn > 0);
	POMDOG_ASSERT(backBufferHeightIn > 0);

	preferredBackBufferWidth = backBufferWidthIn;
	preferredBackBufferHeight = backBufferHeightIn;

	bool isActive = false;

	if (!boundRenderTargets.empty()) {
		isActive = (backBuffer == boundRenderTargets.front());
	}

	POMDOG_ASSERT(backBuffer);
	backBuffer->ResetBackBuffer();

	POMDOG_ASSERT(swapChain);
	auto hr = swapChain->ResizeBuffers(backBufferCount, preferredBackBufferWidth,
		preferredBackBufferHeight, DXGIFormatHelper::ToDXGIFormat(backBufferFormat), 0);

	if (FAILED(hr)) {
		POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to resize back buffer");
	}

	backBuffer->ResetBackBuffer(nativeDevice, swapChain.Get(),
		preferredBackBufferWidth, preferredBackBufferHeight, backBufferDepthFormat);

	if (isActive) {
		SetRenderTarget();
	}
}
//-----------------------------------------------------------------------
ID3D11DeviceContext* GraphicsContextDirect3D11::GetDeviceContext()
{
	POMDOG_ASSERT(deviceContext);
	return deviceContext.Get();
}
//-----------------------------------------------------------------------
}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
