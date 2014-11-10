//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "RenderLayer.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
std::int32_t RenderLayer::DrawOrder() const
{
	return drawOrder;
}
//-----------------------------------------------------------------------
void RenderLayer::DrawOrder(std::int32_t drawOrderIn)
{
	this->drawOrder = drawOrderIn;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
