// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <string>

namespace Pomdog {

struct GamepadCapabilities final {
    std::string Name;

    int ButtonCount = 0;

    int ThumbStickCount = 0;

    bool IsConnected = false;

    bool HasLeftXThumbStick = false;

    bool HasLeftYThumbStick = false;

    bool HasRightXThumbStick = false;

    bool HasRightYThumbStick = false;
};

} // namespace Pomdog
