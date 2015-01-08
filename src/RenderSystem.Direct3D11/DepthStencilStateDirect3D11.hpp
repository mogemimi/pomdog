//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_DEPTHSTENCILSTATEDIRECT3D11_F08A14A0_EDA2_4794_88ED_C43CC4059658_HPP
#define POMDOG_DEPTHSTENCILSTATEDIRECT3D11_F08A14A0_EDA2_4794_88ED_C43CC4059658_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeDepthStencilState.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {

class DepthStencilStateDirect3D11 final: public NativeDepthStencilState {
public:
	DepthStencilStateDirect3D11() = delete;

	DepthStencilStateDirect3D11(ID3D11Device* nativeDevice, DepthStencilDescription const& description);

	///@copydoc NativeDepthStencilState
	void Apply(NativeGraphicsContext & graphicsContext) override;

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> nativeDepthStencilState;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_DEPTHSTENCILSTATEDIRECT3D11_F08A14A0_EDA2_4794_88ED_C43CC4059658_HPP)
