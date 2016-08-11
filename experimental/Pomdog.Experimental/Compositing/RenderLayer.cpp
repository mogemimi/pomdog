// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "RenderLayer.hpp"

namespace Pomdog {

std::int32_t RenderLayer::DrawOrder() const
{
    return drawOrder;
}

void RenderLayer::DrawOrder(std::int32_t drawOrderIn)
{
    this->drawOrder = drawOrderIn;
}

} // namespace Pomdog
