// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SKIN_8B9C736F_D6E4_449A_A310_1F791AA6ABC5_HPP
#define POMDOG_SKIN_8B9C736F_D6E4_449A_A310_1F791AA6ABC5_HPP

#include "RigidSlot.hpp"
#include <cstdint>
#include <vector>

namespace Pomdog {

class Skin {
public:
    Skin() = default;
    explicit Skin(std::vector<RigidSlot> && slots);

    std::vector<RigidSlot> const& Slots() const;

    std::uint16_t FindSlotIndexById(std::uint32_t hashId) const;
    std::uint16_t FindSlotIndexById(std::uint32_t hashId);

    RigidSlot const& SlotByIndex(std::uint16_t slotIndex) const;
    RigidSlot & SlotByIndex(std::uint16_t slotIndex);

private:
    std::vector<RigidSlot> slots;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKIN_8B9C736F_D6E4_449A_A310_1F791AA6ABC5_HPP)
