// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMEPADCAPABILITIES_869064F7_HPP
#define POMDOG_GAMEPADCAPABILITIES_869064F7_HPP

#include "GamepadType.hpp"
#include <cstdint>

namespace Pomdog {

class GamepadCapabilities {
public:
    GamepadType GamepadType = GamepadType::Unknown;

    bool IsConnected = false;

    bool HasLeftXThumbStick = false;

    bool HasLeftYThumbStick = false;

    bool HasRightXThumbStick = false;

    bool HasRightYThumbStick = false;
};

} // namespace Pomdog

#endif // POMDOG_GAMEPADCAPABILITIES_869064F7_HPP
