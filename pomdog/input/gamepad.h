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

    [[nodiscard]] virtual GamepadCapabilities
    getCapabilities(PlayerIndex index) const = 0;

    [[nodiscard]] virtual GamepadState
    getState(PlayerIndex index) const = 0;

    Signal<void(PlayerIndex index, const GamepadCapabilities& caps)> Connected;

    Signal<void(PlayerIndex index, const GamepadCapabilities& caps)> Disconnected;
};

} // namespace pomdog
