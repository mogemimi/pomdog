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

    [[nodiscard]] MouseState
    getState() const override;

    void handleEvent(const SystemEvent& event);

private:
    MouseState state_;
    double scrollWheel_ = 0.0;
};

} // namespace pomdog::detail::cocoa
