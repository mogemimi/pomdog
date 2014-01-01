//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/DepthStencilDescription.hpp>
#include <limits>

namespace Pomdog {

DepthStencilDescription::DepthStencilDescription()
	: ReferenceStencil(0)
	, StencilMask(std::numeric_limits<std::uint32_t>::max())
	, StencilWriteMask(std::numeric_limits<std::uint32_t>::max())
	, DepthBufferFunction(ComparisonFunction::LessEqual)
	, DepthBufferEnable(true)
	, DepthBufferWriteEnable(true)
	, StencilEnable(false)
{}

}// namespace Pomdog
