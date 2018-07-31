// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Input/GamepadUUID.hpp"
#include <string>

namespace Pomdog {

struct GamepadCapabilities final {
    GamepadUUID DeviceUUID;
    std::string Name;

    bool HasAButton = false;
    bool HasBButton = false;
    bool HasXButton = false;
    bool HasYButton = false;
    bool HasLeftShoulderButton = false;
    bool HasRightShoulderButton = false;
    bool HasLeftMenuButton = false;
    bool HasRightMenuButton = false;
    bool HasLeftStickButton = false;
    bool HasRightStickButton = false;
    bool HasGuideButton = false;
    bool HasExtra1Button = false;
    bool HasExtra2Button = false;

    bool HasLeftTrigger = false;
    bool HasRightTrigger = false;

    bool HasLeftXThumbStick = false;
    bool HasLeftYThumbStick = false;
    bool HasRightXThumbStick = false;
    bool HasRightYThumbStick = false;
};

} // namespace Pomdog
