// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/skeletal2d/rigid_slot.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class Skin final {
public:
    Skin() = default;
    explicit Skin(std::vector<RigidSlot>&& slots);

    const std::vector<RigidSlot>& GetSlots() const;

    std::uint16_t FindSlotIndexByID(std::uint32_t hashID) const;
    std::uint16_t FindSlotIndexByID(std::uint32_t hashID);

    const RigidSlot& GetSlotByIndex(std::uint16_t slotIndex) const;
    RigidSlot& GetSlotByIndex(std::uint16_t slotIndex);

private:
    std::vector<RigidSlot> slots;
};

} // namespace pomdog::skeletal2d
