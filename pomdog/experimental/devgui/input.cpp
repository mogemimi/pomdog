// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/input.h"
#include "pomdog/basic/platform.h"
#include "pomdog/experimental/text_input/text_input_service.h"
#include "pomdog/input/keyboard.h"
#include "pomdog/input/keys.h"
#include "pomdog/input/mouse.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/math/point2d.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {

Input makeInput(
    const Mouse& mouse,
    raw_ptr<const Keyboard> keyboard,
    raw_ptr<const textinput::TextInputService> textInput,
    const Vector2& viewportSize,
    f64 timeSeconds,
    f32 uiScale) noexcept
{
    POMDOG_ASSERT(uiScale > 0.0f);
    const auto position = mouse.getPosition();

    Input input;
    input.viewportSize = viewportSize / uiScale;
    input.pointerPosition = Vector2{static_cast<f32>(position.x), static_cast<f32>(position.y)} / uiScale;
    input.wheelDelta = Vector2{
        static_cast<f32>(mouse.getScrollDeltaX()),
        static_cast<f32>(mouse.getScrollDeltaY()),
    };
    input.timeSeconds = timeSeconds;
    input.pointerPresent = mouse.isPresent();
    input.primaryButtonDown = mouse.isButtonDown(MouseButtons::Left);
    input.secondaryButtonDown = mouse.isButtonDown(MouseButtons::Right);

    if (textInput != nullptr) {
        input.textInputEvents = textInput->getEvents();
    }
    if (keyboard == nullptr) {
        return input;
    }

    constexpr std::array<std::pair<EditKey, Keys>, 15> keyMap = {{
        {EditKey::LeftArrow, Keys::LeftArrow},
        {EditKey::RightArrow, Keys::RightArrow},
        {EditKey::UpArrow, Keys::UpArrow},
        {EditKey::DownArrow, Keys::DownArrow},
        {EditKey::Home, Keys::Home},
        {EditKey::End, Keys::End},
        {EditKey::Backspace, Keys::BackSpace},
        {EditKey::Delete, Keys::Delete},
        {EditKey::Enter, Keys::Enter},
        {EditKey::Escape, Keys::Escape},
        {EditKey::Tab, Keys::Tab},
        {EditKey::A, Keys::A},
        {EditKey::C, Keys::C},
        {EditKey::V, Keys::V},
        {EditKey::X, Keys::X},
    }};
    for (const auto& [editKey, key] : keyMap) {
        input.setKeyDown(editKey, keyboard->isKeyDown(key));
    }

    if (textInput == nullptr) {
        input.textInput = keyboard->getTextInput();
    }
    input.shiftDown = keyboard->isShiftKeyDown();
#if defined(POMDOG_PLATFORM_MACOSX)
    input.shortcutDown = keyboard->isSuperKeyDown();
#else
    input.shortcutDown = keyboard->isControlKeyDown();
#endif
    return input;
}

} // namespace pomdog::devgui
