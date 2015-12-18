// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_KEYBOARDWIN32_4403DC3F_HPP
#define POMDOG_KEYBOARDWIN32_4403DC3F_HPP

#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include "Pomdog/Input/Keyboard.hpp"
#include "Pomdog/Input/KeyboardState.hpp"

namespace Pomdog {
namespace Detail {
namespace Win32 {

class KeyboardWin32 final : public Keyboard {
public:
    KeyboardState GetState() const override;

    void HandleMessage(RAWKEYBOARD const& keyboard);

private:
    KeyboardState keyboardState;
};

} // namespace Win32
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_KEYBOARDWIN32_4403DC3F_HPP
