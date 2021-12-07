// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/system_events.hpp"
#include "pomdog/input/keyboard.hpp"
#include "pomdog/input/keyboard_state.hpp"
#include "pomdog/platform/win32/prerequisites_win32.hpp"

namespace Pomdog::Detail::Win32 {

class KeyboardWin32 final : public Keyboard {
public:
    KeyboardState GetState() const override;

    void HandleMessage(const SystemEvent& event);

private:
    KeyboardState keyboardState;
};

void TranslateKeyboardEvent(const RAWKEYBOARD& keyboard, const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue) noexcept;

} // namespace Pomdog::Detail::Win32
