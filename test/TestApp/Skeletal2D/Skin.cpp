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
Skin::Skin(std::vector<RigidSlot> && slotsIn)
	: slots(std::move(slotsIn))
{}
//-----------------------------------------------------------------------
std::vector<RigidSlot> const& Skin::Slots() const
{
	return slots;
}
//-----------------------------------------------------------------------
RigidSlot const& Skin::Slots(std::uint16_t slotIndex) const
{
	POMDOG_ASSERT(slotIndex < slots.size());
	return slots[slotIndex];
}
//-----------------------------------------------------------------------
RigidSlot & Skin::Slots(std::uint16_t slotIndex)
{
	POMDOG_ASSERT(slotIndex < slots.size());
	return slots[slotIndex];
}
//-----------------------------------------------------------------------
}// namespace Pomdog
