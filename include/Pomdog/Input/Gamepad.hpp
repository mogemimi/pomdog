// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_GAMEPAD_5B4AD0B0_HPP
#define POMDOG_GAMEPAD_5B4AD0B0_HPP

#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class GamepadState;
class GamepadCapabilities;

class POMDOG_EXPORT Gamepad {
public:
    virtual ~Gamepad() = default;

    virtual GamepadCapabilities GetCapabilities() const = 0;

    virtual GamepadState GetState() const = 0;
};

} // namespace Pomdog

#endif // POMDOG_GAMEPAD_5B4AD0B0_HPP
