// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/x11/mouse_x11.h"
#include "pomdog/input/backends/mouse_impl.h"
#include "pomdog/input/button_state.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::x11 {
namespace {

[[nodiscard]] std::optional<MouseButtons>
toMouseButton(unsigned int buttonIndex) noexcept
{
    switch (buttonIndex) {
    case Button1:
        return MouseButtons::Left;
    case Button2:
        return MouseButtons::Middle;
    case Button3:
        return MouseButtons::Right;
    default:
        break;
    }
    return std::nullopt;
}

} // namespace

MouseX11::MouseX11(const std::shared_ptr<MouseImpl>& impl)
    : impl_(impl)
{
    POMDOG_ASSERT(impl_ != nullptr);
}

void MouseX11::handleEvent(XEvent& event)
{
    POMDOG_ASSERT(impl_ != nullptr);

    switch (event.type) {
    case ButtonPress: {
        if (auto button = toMouseButton(event.xbutton.button); button.has_value()) {
            impl_->setButton(*button, ButtonState::Down);
        }
        else if (event.xbutton.button == Button4) {
            // Scroll up
            impl_->addScroll(0.0, 1.0);
        }
        else if (event.xbutton.button == Button5) {
            // Scroll down
            impl_->addScroll(0.0, -1.0);
        }
        else if (event.xbutton.button == 6) {
            // Scroll left
            impl_->addScroll(-1.0, 0.0);
        }
        else if (event.xbutton.button == 7) {
            // Scroll right
            impl_->addScroll(1.0, 0.0);
        }
        break;
    }
    case ButtonRelease: {
        if (auto button = toMouseButton(event.xbutton.button); button.has_value()) {
            impl_->setButton(*button, ButtonState::Up);
        }
        break;
    }
    case EnterNotify: {
        break;
    }
    case MotionNotify: {
        impl_->setPosition(Point2D{event.xmotion.x, event.xmotion.y});
        break;
    }
    case LeaveNotify: {
        break;
    }
    }
}

} // namespace pomdog::detail::x11
