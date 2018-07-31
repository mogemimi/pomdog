// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Input/Gamepad.hpp"

namespace Pomdog {
namespace Detail {
namespace InputSystem {

class NativeGamepad : public Gamepad {
public:
    virtual ~NativeGamepad() = default;

    virtual void EnumerateDevices() = 0;

    virtual void PollEvents() = 0;
};

} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog
