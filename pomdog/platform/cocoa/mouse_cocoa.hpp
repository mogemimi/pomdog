// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/mouse.hpp"
#include "pomdog/input/mouse_state.hpp"

namespace Pomdog::Detail {
class SystemEvent;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Cocoa {

class MouseCocoa final : public Mouse {
public:
    MouseCocoa();

    MouseState GetState() const override;

    void HandleEvent(const SystemEvent& event);

private:
    MouseState state;
    double scrollWheel;
};

} // namespace Pomdog::Detail::Cocoa
