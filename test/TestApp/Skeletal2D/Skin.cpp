//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Skin.hpp"
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
Skin::Skin(std::vector<ModelSlot2D> && slotsIn)
	: slots(std::move(slotsIn))
{}
//-----------------------------------------------------------------------
std::vector<ModelSlot2D> const& Skin::Slots() const
{
	return slots;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
