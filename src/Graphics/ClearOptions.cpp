// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Graphics/ClearOptions.hpp"

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
