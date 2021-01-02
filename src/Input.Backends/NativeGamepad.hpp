// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Input/Gamepad.hpp"

namespace Pomdog::Detail {

class NativeGamepad : public Gamepad {
public:
    virtual ~NativeGamepad() override;

    virtual void EnumerateDevices() = 0;

    virtual void PollEvents() = 0;
};

} // namespace Pomdog::Detail
