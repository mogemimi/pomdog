//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Skin.hpp"
#include <utility>
#include <algorithm>

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
std::uint16_t Skin::FindSlotIndexById(std::uint32_t hashId) const
{
	auto iter = std::find_if(std::begin(slots), std::end(slots),
		[&hashId](RigidSlot const& slot){ return slot.HashId == hashId; });
	POMDOG_ASSERT(iter != std::end(slots));
	return std::distance(std::begin(slots), iter);
}
//-----------------------------------------------------------------------
std::uint16_t Skin::FindSlotIndexById(std::uint32_t hashId)
{
	auto iter = std::find_if(std::begin(slots), std::end(slots),
		[&hashId](RigidSlot const& slot){ return slot.HashId == hashId; });
	POMDOG_ASSERT(iter != std::end(slots));
	return std::distance(std::begin(slots), iter);
}
//-----------------------------------------------------------------------
RigidSlot const& Skin::SlotByIndex(std::uint16_t slotIndex) const
{
	POMDOG_ASSERT(slotIndex < slots.size());
	return slots[slotIndex];
}
//-----------------------------------------------------------------------
RigidSlot & Skin::SlotByIndex(std::uint16_t slotIndex)
{
	POMDOG_ASSERT(slotIndex < slots.size());
	return slots[slotIndex];
}
//-----------------------------------------------------------------------
}// namespace Pomdog
