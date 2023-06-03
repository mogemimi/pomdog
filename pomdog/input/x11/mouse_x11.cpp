// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/x11/mouse_x11.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::x11 {
namespace {

[[nodiscard]] ButtonState*
getButtonByIndex(MouseState& mouseState, unsigned int buttonIndex) noexcept
{
    switch (buttonIndex) {
    case Button1:
        return &mouseState.leftButton;
    case Button2:
        return &mouseState.middleButton;
    case Button3:
        return &mouseState.rightButton;
    default:
        break;
    }
    return nullptr;
}

[[nodiscard]] std::optional<MouseButtons>
toMouseButtons(unsigned int buttonIndex) noexcept
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

MouseState MouseX11::getState() const
{
    return mouseState_;
}

void MouseX11::handleEvent(XEvent& event)
{
    switch (event.type) {
    case ButtonPress: {
        if (auto button = getButtonByIndex(mouseState_, event.xbutton.button); button != nullptr) {
            *button = ButtonState::Pressed;

            auto mouseButton = toMouseButtons(event.xbutton.button);
            POMDOG_ASSERT(mouseButton);
            Mouse::ButtonDown(*mouseButton);
        }
        else if (event.xbutton.button == Button4) {
            const auto previousScrollWheel = mouseState_.scrollWheel;
            mouseState_.scrollWheel += 1;
            Mouse::ScrollWheel(mouseState_.scrollWheel - previousScrollWheel);
        }
        else if (event.xbutton.button == Button5) {
            const auto previousScrollWheel = mouseState_.scrollWheel;
            mouseState_.scrollWheel -= 1;
            Mouse::ScrollWheel(mouseState_.scrollWheel - previousScrollWheel);
        }
        break;
    }
    case ButtonRelease: {
        if (auto button = getButtonByIndex(mouseState_, event.xbutton.button); button != nullptr) {
            *button = ButtonState::Released;

            auto mouseButton = toMouseButtons(event.xbutton.button);
            POMDOG_ASSERT(mouseButton);
            Mouse::ButtonUp(*mouseButton);
        }
        break;
    }
    case EnterNotify: {
        break;
    }
    case MotionNotify: {
        mouseState_.position.x = event.xmotion.x;
        mouseState_.position.y = event.xmotion.y;
        Mouse::Moved(mouseState_.position);
        break;
    }
    case LeaveNotify: {
        break;
    }
    }
}

} // namespace pomdog::detail::x11
