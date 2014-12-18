//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include <Pomdog/Graphics/ClearOptions.hpp>

namespace Pomdog {

ClearOptions & operator|=(ClearOptions & a, ClearOptions b)
{
	static_assert(ClearOptions::ClearOptions_Details_DepthStecil == (
		ClearOptions::DepthBuffer | ClearOptions::Stencil), "");
	static_assert(ClearOptions::ClearOptions_Details_TargetStencil == (
		ClearOptions::RenderTarget | ClearOptions::Stencil), "");
	static_assert(ClearOptions::ClearOptions_Details_TargetDepth == (
		ClearOptions::DepthBuffer | ClearOptions::RenderTarget), "");
	static_assert(ClearOptions::ClearOptions_Details_TargetDepthStencil == (
		ClearOptions::RenderTarget | ClearOptions::DepthBuffer | ClearOptions::Stencil), "");

	a = a | b;
	return a;
}

}// namespace Pomdog
