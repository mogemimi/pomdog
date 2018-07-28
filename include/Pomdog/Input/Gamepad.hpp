// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Input/PlayerIndex.hpp"

namespace Pomdog {

struct GamepadState;
struct GamepadCapabilities;

class POMDOG_EXPORT Gamepad {
public:
    virtual ~Gamepad() = default;

    virtual GamepadCapabilities GetCapabilities(PlayerIndex index) const = 0;

    virtual GamepadState GetState(PlayerIndex index) const = 0;
};

} // namespace Pomdog
