//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_EFFECTREFLECTIONDIRECT3D11_10267AA4_F5EC_4BC4_A092_51877E2D340F_HPP
#define POMDOG_EFFECTREFLECTIONDIRECT3D11_10267AA4_F5EC_4BC4_A092_51877E2D340F_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeEffectReflection.hpp"
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>

namespace Pomdog {
namespace Details {

class ShaderBytecode;

namespace RenderSystem {
namespace Direct3D11 {

class EffectReflectionDirect3D11 final: public NativeEffectReflection {
public:
	EffectReflectionDirect3D11(ShaderBytecode const& vertexShaderBytecode,
		ShaderBytecode const& pixelShaderBytecode);

	std::vector<EffectConstantDescription> GetConstantBuffers() const override;

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> vertexShaderReflector;
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pixelShaderReflector;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTREFLECTIONDIRECT3D11_10267AA4_F5EC_4BC4_A092_51877E2D340F_HPP)
