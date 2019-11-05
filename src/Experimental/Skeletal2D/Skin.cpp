// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Skeletal2D/Skin.hpp"
#include <algorithm>
#include <utility>

namespace Pomdog::Skeletal2D {

Skin::Skin(std::vector<RigidSlot>&& slotsIn)
    : slots(std::move(slotsIn))
{
}

const std::vector<RigidSlot>& Skin::GetSlots() const
{
    return slots;
}

std::uint16_t Skin::FindSlotIndexByID(std::uint32_t hashID) const
{
    auto iter = std::find_if(std::begin(slots), std::end(slots),
        [&hashID](const RigidSlot& slot) { return slot.HashID == hashID; });
    POMDOG_ASSERT(iter != std::end(slots));
    auto slotIndex = std::distance(std::begin(slots), iter);
    POMDOG_ASSERT(slotIndex >= 0);
    POMDOG_ASSERT(slotIndex <= std::numeric_limits<std::uint16_t>::max());
    return static_cast<std::uint16_t>(slotIndex);
}

std::uint16_t Skin::FindSlotIndexByID(std::uint32_t hashID)
{
    auto iter = std::find_if(std::begin(slots), std::end(slots),
        [&hashID](const RigidSlot& slot) { return slot.HashID == hashID; });
    POMDOG_ASSERT(iter != std::end(slots));
    const auto slotIndex = std::distance(std::begin(slots), iter);
    POMDOG_ASSERT(slotIndex >= 0);
    POMDOG_ASSERT(slotIndex <= std::numeric_limits<std::uint16_t>::max());
    return static_cast<std::uint16_t>(slotIndex);
}

const RigidSlot& Skin::GetSlotByIndex(std::uint16_t slotIndex) const
{
    POMDOG_ASSERT(slotIndex < slots.size());
    return slots[slotIndex];
}

RigidSlot& Skin::GetSlotByIndex(std::uint16_t slotIndex)
{
    POMDOG_ASSERT(slotIndex < slots.size());
    return slots[slotIndex];
}

} // namespace Pomdog::Skeletal2D
