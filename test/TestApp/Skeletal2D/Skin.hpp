//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SKIN_8B9C736F_D6E4_449A_A310_1F791AA6ABC5_HPP
#define POMDOG_SKIN_8B9C736F_D6E4_449A_A310_1F791AA6ABC5_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <vector>
#include "RigidSlot.hpp"

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
