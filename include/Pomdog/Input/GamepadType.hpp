// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class GamepadType : std::uint8_t {
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
