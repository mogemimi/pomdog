//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_RENDERTARGET2DDIRECT3D11_75264088_5BBA_4784_95F9_9717260F6FF8_HPP
#define POMDOG_RENDERTARGET2DDIRECT3D11_75264088_5BBA_4784_95F9_9717260F6FF8_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeRenderTarget2D.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {

class RenderTarget2DDirect3D11 final: public NativeRenderTarget2D {
public:
	RenderTarget2DDirect3D11(ID3D11Device* nativeDevice,
		std::int32_t pixelWidth, std::int32_t pixelHeight,
		std::uint32_t levelCount, SurfaceFormat format, DepthFormat depthStencilFormat);

	RenderTarget2DDirect3D11(ID3D11Device* nativeDevice, IDXGISwapChain* swapChain,
		std::int32_t pixelWidth, std::int32_t pixelHeight, DepthFormat depthStencilFormat);

	void Apply(std::uint32_t index) override;

	ID3D11RenderTargetView* RenderTargetView() const;

	ID3D11DepthStencilView* DepthStencilView() const;

	ID3D11ShaderResourceView* ShaderResourceView() const;

	void ResetBackBuffer(ID3D11Device* nativeDevice, IDXGISwapChain* swapChain,
		std::int32_t pixelWidth, std::int32_t pixelHeight, DepthFormat depthStencilFormat);

	void ResetBackBuffer();

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTexture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureResourceView;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_RENDERTARGET2DDIRECT3D11_75264088_5BBA_4784_95F9_9717260F6FF8_HPP)
