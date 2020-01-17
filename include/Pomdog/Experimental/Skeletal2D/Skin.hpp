// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/RigidSlot.hpp"
#include <cstdint>
#include <vector>

namespace Pomdog::Skeletal2D {

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

} // namespace Pomdog::Skeletal2D
