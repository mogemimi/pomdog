// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_KEYBOARDCOCOA_E0980F30_HPP
#define POMDOG_KEYBOARDCOCOA_E0980F30_HPP

#include "Pomdog/Input/Keyboard.hpp"
#include "Pomdog/Input/KeyboardState.hpp"

namespace Pomdog {
namespace Detail {
namespace Cocoa {

class KeyboardCocoa final : public Keyboard {
public:
    KeyboardCocoa();

    KeyboardState GetState() const override;

    void SetKey(Keys key, KeyState keyState);

private:
    KeyboardState state;
};

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_KEYBOARDCOCOA_E0980F30_HPP
