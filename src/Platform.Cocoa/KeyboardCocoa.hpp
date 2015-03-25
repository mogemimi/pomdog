// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_KEYBOARDCOCOA_E0980F30_9516_49D4_80A0_CCE22DAB9F05_HPP
#define POMDOG_KEYBOARDCOCOA_E0980F30_9516_49D4_80A0_CCE22DAB9F05_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Input/Keyboard.hpp"
#include "Pomdog/Input/KeyboardState.hpp"

namespace Pomdog {
namespace Detail {
namespace Cocoa {

class KeyboardCocoa final: public Keyboard {
public:
    KeyboardCocoa();

    ///@copydoc Pomdog::Mouse
    KeyboardState GetState() const override;

    void SetKey(Keys key, KeyState keyState);

private:
    KeyboardState state;
};

}// namespace Cocoa
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_KEYBOARDCOCOA_E0980F30_9516_49D4_80A0_CCE22DAB9F05_HPP)
