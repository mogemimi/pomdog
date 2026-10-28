// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/input.h"
#include "pomdog/basic/platform.h"
#include "pomdog/experimental/text_input/text_input_service.h"
#include "pomdog/input/keyboard.h"
#include "pomdog/input/keys.h"
#include "pomdog/input/mouse.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/input/touchscreen.h"
#include "pomdog/math/point2d.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <utility>
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include <emscripten.h>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
// NOTE: EM_JS defines variables that only the linker reads, without declarations.
POMDOG_EMCC_SUPPRESS_WARNING_PUSH
POMDOG_EMCC_SUPPRESS_WARNING("-Wmissing-variable-declarations")

// clang-format off
EM_JS(int, pomdog_devgui_uses_super_shortcuts, (), {
    if (typeof navigator === "undefined") {
        return 0;
    }
    var platform = (navigator.userAgentData && navigator.userAgentData.platform) || navigator.platform || "";
    return /^(mac|iphone|ipad|ipod|ios)/i.test(platform);
});
// clang-format on

POMDOG_EMCC_SUPPRESS_WARNING_POP
#endif

namespace pomdog::devgui::detail {

bool hostUsesSuperShortcuts() noexcept
{
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    // NOTE: Other files reach the browser query through this function. A call
    // to the EM_JS function from another file links without LTO, but with LTO
    // the linker reports it as an undefined symbol.
    return pomdog_devgui_uses_super_shortcuts() != 0;
#elif defined(POMDOG_PLATFORM_MACOSX)
    return true;
#else
    return false;
#endif
}

} // namespace pomdog::devgui::detail

namespace pomdog::devgui {

Input makeInput(
    const Mouse& mouse,
    raw_ptr<const Touchscreen> touchscreen,
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

    if (touchscreen != nullptr) {
        auto center = Vector2{0.0f, 0.0f};
        u8 touchCount = 0;
        for (u8 i = 0; i < touchscreen->getSupportedTouchCount(); ++i) {
            if (touchscreen->isTouchPressed(i)) {
                const auto touch = touchscreen->getPosition(i);
                center += Vector2{static_cast<f32>(touch.x), static_cast<f32>(touch.y)};
                touchCount += 1;
            }
        }
        if (touchCount > 0) {
            // NOTE: Fingers on the screen take the pointer from the mouse.
            input.pointerType = PointerType::Touch;
            input.touchCount = touchCount;
            input.pointerPosition = center / (static_cast<f32>(touchCount) * uiScale);
            input.pointerPresent = true;
            input.primaryButtonDown = true;
        }
    }

    // NOTE: The host does not change while the application runs, so the
    // browser is asked once.
    static const bool shortcutUsesSuper = detail::hostUsesSuperShortcuts();
    input.shortcutUsesSuper = shortcutUsesSuper;

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
    if (input.shortcutUsesSuper) {
        input.shortcutDown = keyboard->isSuperKeyDown();
    }
    else {
        input.shortcutDown = keyboard->isControlKeyDown();
    }
    return input;
}

} // namespace pomdog::devgui
