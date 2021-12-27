// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/gamepad.h"

namespace pomdog::detail {

class NativeGamepad : public Gamepad {
public:
    virtual ~NativeGamepad() override;

    virtual void EnumerateDevices() = 0;

    virtual void PollEvents() = 0;
};

} // namespace pomdog::detail
