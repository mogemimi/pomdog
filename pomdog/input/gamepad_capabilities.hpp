// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/input/gamepad_uuid.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
