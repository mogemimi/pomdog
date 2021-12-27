// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/system_events.h"
#include "pomdog/input/keyboard.h"
#include "pomdog/input/keyboard_state.h"
#include "pomdog/platform/win32/prerequisites_win32.h"

namespace pomdog::detail::win32 {

class KeyboardWin32 final : public Keyboard {
public:
    KeyboardState GetState() const override;

    void HandleMessage(const SystemEvent& event);

private:
    KeyboardState keyboardState;
};

void TranslateKeyboardEvent(const RAWKEYBOARD& keyboard, const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue) noexcept;

} // namespace pomdog::detail::win32
