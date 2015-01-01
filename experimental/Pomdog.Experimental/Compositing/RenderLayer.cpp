//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
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
