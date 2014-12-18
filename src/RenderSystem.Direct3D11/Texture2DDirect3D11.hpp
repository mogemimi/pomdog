//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_TEXTURE2DDIRECT3D11_BA251B9A_30C1_4F55_ACF2_AAE304E8EFFA_HPP
#define POMDOG_TEXTURE2DDIRECT3D11_BA251B9A_30C1_4F55_ACF2_AAE304E8EFFA_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeTexture2D.hpp"
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {

class Texture2DDirect3D11 final: public NativeTexture2D {
public:
	Texture2DDirect3D11(ID3D11Device* nativeDevice,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> const& deviceContext,
		std::int32_t pixelWidth, std::int32_t pixelHeight,
		std::uint32_t levelCount, SurfaceFormat format);

	void SetData(std::int32_t pixelWidth, std::int32_t pixelHeight,
		std::uint32_t levelCount, SurfaceFormat format, void const* pixelData) override;

	///@copydoc NativeTexture2D
	void Apply(std::uint32_t index) override;

	ID3D11ShaderResourceView* ShaderResourceView() const;

private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> nativeTexture2D;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_TEXTURE2DDIRECT3D11_BA251B9A_30C1_4F55_ACF2_AAE304E8EFFA_HPP)
