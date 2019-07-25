// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Skin.hpp"
#include <utility>
#include <algorithm>

namespace Pomdog {

Skin::Skin(std::vector<RigidSlot> && slotsIn)
    : slots(std::move(slotsIn))
{}

std::vector<RigidSlot> const& Skin::Slots() const
{
    return slots;
}

std::uint16_t Skin::FindSlotIndexById(std::uint32_t hashId) const
{
    auto iter = std::find_if(std::begin(slots), std::end(slots),
        [&hashId](RigidSlot const& slot){ return slot.HashId == hashId; });
    POMDOG_ASSERT(iter != std::end(slots));
    auto slotIndex = std::distance(std::begin(slots), iter);
    POMDOG_ASSERT(slotIndex >= 0);
    POMDOG_ASSERT(slotIndex <= std::numeric_limits<std::uint16_t>::max());
    return static_cast<std::uint16_t>(slotIndex);
}

std::uint16_t Skin::FindSlotIndexById(std::uint32_t hashId)
{
    auto iter = std::find_if(std::begin(slots), std::end(slots),
        [&hashId](RigidSlot const& slot){ return slot.HashId == hashId; });
    POMDOG_ASSERT(iter != std::end(slots));
    const auto slotIndex = std::distance(std::begin(slots), iter);
    POMDOG_ASSERT(slotIndex >= 0);
    POMDOG_ASSERT(slotIndex <= std::numeric_limits<std::uint16_t>::max());
    return static_cast<std::uint16_t>(slotIndex);
}

RigidSlot const& Skin::SlotByIndex(std::uint16_t slotIndex) const
{
    POMDOG_ASSERT(slotIndex < slots.size());
    return slots[slotIndex];
}

RigidSlot & Skin::SlotByIndex(std::uint16_t slotIndex)
{
    POMDOG_ASSERT(slotIndex < slots.size());
    return slots[slotIndex];
}

} // namespace Pomdog
