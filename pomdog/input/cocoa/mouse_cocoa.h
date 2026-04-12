// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/backends/mouse_state.h"
#include "pomdog/input/mouse.h"

namespace pomdog::detail {
class SystemEvent;
} // namespace pomdog::detail

namespace pomdog::detail::cocoa {

class MouseCocoa final : public Mouse {
public:
    MouseCocoa();

    [[nodiscard]] Point2D
    getPosition() const noexcept override;

    [[nodiscard]] bool
    isButtonDown(MouseButtons button) const noexcept override;

    [[nodiscard]] i32
    getScrollX() const noexcept override;

    [[nodiscard]] i32
    getScrollY() const noexcept override;

    [[nodiscard]] bool
    isPresent() const noexcept override;

    void handleEvent(const SystemEvent& event);

    void clearAllButtons() noexcept;

private:
    MouseState state_;
    double scrollWheel_ = 0.0;
};

} // namespace pomdog::detail::cocoa
