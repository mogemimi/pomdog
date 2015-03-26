// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_KEYBOARD_B6F4D4B4_HPP
#define POMDOG_KEYBOARD_B6F4D4B4_HPP

#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class KeyboardState;

class POMDOG_EXPORT Keyboard {
public:
    Keyboard() = default;
    Keyboard(Keyboard const&) = delete;
    Keyboard & operator=(Keyboard const&) = delete;

    virtual ~Keyboard() = default;

    virtual KeyboardState GetState() const = 0;
};

}// namespace Pomdog

#endif // POMDOG_KEYBOARD_B6F4D4B4_HPP
