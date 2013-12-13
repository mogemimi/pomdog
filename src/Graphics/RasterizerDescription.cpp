//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Graphics/RasterizerDescription.hpp>

namespace Pomdog {

RasterizerDescription::RasterizerDescription()
	: DepthBias(0)
	, SlopeScaledDepthBias(0.0f)
	, CullMode(CullMode::CounterClockwiseFace)
	, FillMode(FillMode::Solid)
	, MultisampleEnable(false)
	, ScissorTestEnable(false)
{}

}// namespace Pomdog
