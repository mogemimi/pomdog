//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_RASTERIZERSTATEDIRECT3D11_35B2E2F8_2BA8_4906_A6F9_D2273F71F45C_HPP
#define POMDOG_RASTERIZERSTATEDIRECT3D11_35B2E2F8_2BA8_4906_A6F9_D2273F71F45C_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeRasterizerState.hpp"
#include <Pomdog/Graphics/detail/ForwardDeclarations.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace Direct3D11 {

class RasterizerStateDirect3D11 final: public NativeRasterizerState {
public:
	RasterizerStateDirect3D11() = delete;

	RasterizerStateDirect3D11(ID3D11Device* nativeDevice, RasterizerDescription const& description);

	///@copydoc NativeRasterizerState
	void Apply(NativeGraphicsContext & graphicsContext) override;

private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> nativeRasterizerState;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_RASTERIZERSTATEDIRECT3D11_35B2E2F8_2BA8_4906_A6F9_D2273F71F45C_HPP)
