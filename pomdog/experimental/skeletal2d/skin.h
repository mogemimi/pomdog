// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/rigid_slot.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class Skin final {
public:
    Skin() = default;
    explicit Skin(std::vector<RigidSlot>&& slots);

    const std::vector<RigidSlot>& getSlots() const;

    u16 findSlotIndexByID(u32 hashID) const;
    u16 findSlotIndexByID(u32 hashID);

    const RigidSlot& getSlotByIndex(u16 slotIndex) const;
    RigidSlot& getSlotByIndex(u16 slotIndex);

private:
    std::vector<RigidSlot> slots_;
};

} // namespace pomdog::skeletal2d
