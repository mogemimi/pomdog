// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_KEYBOARD_B6F4D4B4_HPP
#define POMDOG_KEYBOARD_B6F4D4B4_HPP

#include "Pomdog/Signals/Signal.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <string>

namespace Pomdog {

enum class Keys: std::uint8_t;
class KeyboardState;

class POMDOG_EXPORT Keyboard {
public:
    Keyboard() = default;
    Keyboard(Keyboard const&) = delete;
    Keyboard & operator=(Keyboard const&) = delete;

    virtual ~Keyboard() = default;

    virtual KeyboardState GetState() const = 0;

    Signal<void(Keys key)> KeyDown;

    Signal<void(Keys key)> KeyUp;

    Signal<void(std::string const& text)> TextInput;
};

} // namespace Pomdog

#endif // POMDOG_KEYBOARD_B6F4D4B4_HPP
