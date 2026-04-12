// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/system_events.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/backends/keyboard_state.h"
#include "pomdog/input/keyboard.h"
#include "pomdog/platform/win32/prerequisites_win32.h"
#include "pomdog/signals/event_queue.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::win32 {

class KeyboardWin32 final : public Keyboard {
private:
    KeyboardState keyboardState_;
    std::string textInput_;

public:
    [[nodiscard]] bool
    isKeyDown(Keys key) const noexcept override;

    [[nodiscard]] bool
    isKeyUp(Keys key) const noexcept override;

    [[nodiscard]] bool
    isControlKeyDown() const noexcept override;

    [[nodiscard]] bool
    isShiftKeyDown() const noexcept override;

    [[nodiscard]] bool
    isAltKeyDown() const noexcept override;

    [[nodiscard]] bool
    isSuperKeyDown() const noexcept override;

    [[nodiscard]] bool
    isAnyKeyDown() const noexcept override;

    [[nodiscard]] std::string_view
    getTextInput() const noexcept override;

    void handleMessage(const SystemEvent& event);

    void clearAllKeys() noexcept;

    void clearTextInput() noexcept;
};

void translateKeyboardEvent(const RAWKEYBOARD& keyboard, const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue) noexcept;

} // namespace pomdog::detail::win32
