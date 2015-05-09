// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMEPADTYPE_7A74879B_HPP
#define POMDOG_GAMEPADTYPE_7A74879B_HPP

#include <cstdint>

namespace Pomdog {

enum class GamepadType: std::uint8_t {
    Unknown,
    Gamepad,
    ArcadeStick,
    DancePad,
    DrumKit,
    FlightStick,
    Xbox360Pad,
    XboxOnePad,
};

} // namespace Pomdog

#endif // POMDOG_GAMEPADTYPE_7A74879B_HPP
