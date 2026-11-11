// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/skin.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

Skin::Skin(std::vector<RigidSlot>&& slotsIn)
    : slots_(std::move(slotsIn))
{
}

const std::vector<RigidSlot>& Skin::getSlots() const
{
    return slots_;
}

u16 Skin::findSlotIndexByID(u32 hashID) const
{
    auto iter = std::find_if(std::begin(slots_), std::end(slots_),
        [&hashID](const RigidSlot& slot) { return slot.hashID == hashID; });
    POMDOG_ASSERT(iter != std::end(slots_));
    auto slotIndex = std::distance(std::begin(slots_), iter);
    POMDOG_ASSERT(slotIndex >= 0);
    POMDOG_ASSERT(slotIndex <= std::numeric_limits<u16>::max());
    return static_cast<u16>(slotIndex);
}

u16 Skin::findSlotIndexByID(u32 hashID)
{
    auto iter = std::find_if(std::begin(slots_), std::end(slots_),
        [&hashID](const RigidSlot& slot) { return slot.hashID == hashID; });
    POMDOG_ASSERT(iter != std::end(slots_));
    const auto slotIndex = std::distance(std::begin(slots_), iter);
    POMDOG_ASSERT(slotIndex >= 0);
    POMDOG_ASSERT(slotIndex <= std::numeric_limits<u16>::max());
    return static_cast<u16>(slotIndex);
}

const RigidSlot& Skin::getSlotByIndex(u16 slotIndex) const
{
    POMDOG_ASSERT(slotIndex < slots_.size());
    return slots_[slotIndex];
}

RigidSlot& Skin::getSlotByIndex(u16 slotIndex)
{
    POMDOG_ASSERT(slotIndex < slots_.size());
    return slots_[slotIndex];
}

} // namespace pomdog::skeletal2d
