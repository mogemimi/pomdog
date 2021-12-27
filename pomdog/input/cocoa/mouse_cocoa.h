// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/mouse.h"
#include "pomdog/input/mouse_state.h"

namespace pomdog::detail {
class SystemEvent;
} // namespace pomdog::detail

namespace pomdog::detail::cocoa {

class MouseCocoa final : public Mouse {
public:
    MouseCocoa();

    MouseState GetState() const override;

    void HandleEvent(const SystemEvent& event);

private:
    MouseState state;
    double scrollWheel;
};

} // namespace pomdog::detail::cocoa
