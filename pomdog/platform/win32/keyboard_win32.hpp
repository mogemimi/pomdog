// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "../Application/SystemEvents.hpp"
#include "Pomdog/Input/Keyboard.hpp"
#include "Pomdog/Input/KeyboardState.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"

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
