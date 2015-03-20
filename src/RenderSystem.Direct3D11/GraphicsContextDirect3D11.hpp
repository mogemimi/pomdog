//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_GRAPHICSCONTEXTDIRECT3D11_596CF1CC_F17F_44AB_9185_812C9D43D191_HPP
#define POMDOG_GRAPHICSCONTEXTDIRECT3D11_596CF1CC_F17F_44AB_9185_812C9D43D191_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeGraphicsContext.hpp"
#include <memory>
#include <vector>
#include <array>

namespace Pomdog {
namespace Detail {
namespace Win32 {

class GameWindowWin32;

}// namespace Win32
namespace RenderSystem {
namespace Direct3D11 {

class GraphicsContextDirect3D11 final : public NativeGraphicsContext {
public:
	GraphicsContextDirect3D11() = delete;

	GraphicsContextDirect3D11(
		HWND windowHandle,
		Microsoft::WRL::ComPtr<IDXGIFactory1> const& dxgiFactory,
		Microsoft::WRL::ComPtr<ID3D11Device> const& nativeDevice,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContext);

	~GraphicsContextDirect3D11();

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void Clear(Color const& color) override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void Clear(ClearOptions options, Color const& color, float depth, std::int32_t stencil) override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void Present() override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void Draw(PrimitiveTopology primitiveTopology, std::uint32_t vertexCount) override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void DrawIndexed(PrimitiveTopology primitiveTopology,
		std::shared_ptr<IndexBuffer> const& indexBuffer, std::uint32_t indexCount) override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void DrawInstanced(PrimitiveTopology primitiveTopology, std::uint32_t vertexCount, std::uint32_t instanceCount) override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void DrawIndexedInstanced(PrimitiveTopology primitiveTopology,
		std::shared_ptr<IndexBuffer> const& indexBuffer, std::uint32_t indexCount, std::uint32_t instanceCount) override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	GraphicsCapabilities GetCapabilities() const override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void SetViewport(Viewport const& viewport) override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	Rectangle GetScissorRectangle() const override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void SetScissorRectangle(Rectangle const& rectangle) override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffers) override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void SetTexture(std::uint32_t index) override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void SetTexture(std::uint32_t index, Texture2D & texture) override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void SetTexture(std::uint32_t index, RenderTarget2D & texture) override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void SetRenderTarget() override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets) override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void SetEffectPass(std::shared_ptr<NativeEffectPass> const& nativeEffectPass) override;

	///@copydoc Pomdog::Detail::RenderSystem::NativeGraphicsContext
	void SetConstantBuffers(std::shared_ptr<NativeConstantLayout> const& nativeConstantLayout) override;

	void ResizeBackBuffers(ID3D11Device* nativeDevice, int backBufferWidth, int backBufferHeight);

public:
	ID3D11DeviceContext* GetDeviceContext();

private:
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>	deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	std::vector<std::shared_ptr<RenderTarget2DDirect3D11>> boundRenderTargets;
	static constexpr std::size_t MaxTextureCount = 8;
	std::array<ID3D11ShaderResourceView*, MaxTextureCount> boundTextureViews;
	std::shared_ptr<RenderTarget2DDirect3D11> backBuffer;
	int preferredBackBufferWidth;
	int preferredBackBufferHeight;
	UINT backBufferCount;
	SurfaceFormat backBufferFormat;
	DepthFormat backBufferDepthFormat;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_GRAPHICSCONTEXTDIRECT3D11_596CF1CC_F17F_44AB_9185_812C9D43D191_HPP)
