//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SAMPLERSTATEDIRECT3D11_7E21EF86_A4C5_4EE8_8B62_33CCC306F202_HPP
#define POMDOG_SAMPLERSTATEDIRECT3D11_7E21EF86_A4C5_4EE8_8B62_33CCC306F202_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeSamplerState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {

class SamplerStateDirect3D11 final: public NativeSamplerState {
public:
	SamplerStateDirect3D11() = delete;

	SamplerStateDirect3D11(ID3D11Device* nativeDevice, SamplerDescription const& description);

	///@copydoc NativeSamplerState
	void Apply(NativeGraphicsContext & graphicsContext, std::uint32_t index) override;

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> nativeSamplerState;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SAMPLERSTATEDIRECT3D11_7E21EF86_A4C5_4EE8_8B62_33CCC306F202_HPP)
