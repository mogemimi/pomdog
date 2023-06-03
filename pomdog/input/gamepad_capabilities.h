// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/gamepad_uuid.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

struct GamepadCapabilities final {
    GamepadUUID deviceUUID;
    std::string name;

    bool hasAButton = false;
    bool hasBButton = false;
    bool hasXButton = false;
    bool hasYButton = false;
    bool hasLeftShoulderButton = false;
    bool hasRightShoulderButton = false;
    bool hasLeftMenuButton = false;
    bool hasRightMenuButton = false;
    bool hasLeftStickButton = false;
    bool hasRightStickButton = false;
    bool hasGuideButton = false;
    bool hasExtra1Button = false;
    bool hasExtra2Button = false;

    bool hasLeftTrigger = false;
    bool hasRightTrigger = false;

    bool hasLeftXThumbStick = false;
    bool hasLeftYThumbStick = false;
    bool hasRightXThumbStick = false;
    bool hasRightYThumbStick = false;
};

} // namespace pomdog
