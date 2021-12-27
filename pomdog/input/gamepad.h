// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/input/player_index.h"
#include "pomdog/signals/signal.h"

namespace pomdog {

struct GamepadState;
struct GamepadCapabilities;

class POMDOG_EXPORT Gamepad {
public:
    virtual ~Gamepad();

    virtual GamepadCapabilities GetCapabilities(PlayerIndex index) const = 0;

    virtual GamepadState GetState(PlayerIndex index) const = 0;

    Signal<void(PlayerIndex index, const GamepadCapabilities& caps)> Connected;

    Signal<void(PlayerIndex index, const GamepadCapabilities& caps)> Disconnected;
};

} // namespace pomdog
