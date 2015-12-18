// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

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
