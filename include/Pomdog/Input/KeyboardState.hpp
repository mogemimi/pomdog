// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_KEYBOARDSTATE_8518E1F7_HPP
#define POMDOG_KEYBOARDSTATE_8518E1F7_HPP

#include "Pomdog/Basic/Export.hpp"
#include <bitset>

namespace Pomdog {

enum class KeyState: bool;
enum class Keys: std::uint8_t;

class POMDOG_EXPORT KeyboardState {
public:
    KeyState operator[](Keys key) const;

    bool IsKeyDown(Keys key) const;

    bool IsKeyUp(Keys key) const;

    void SetKey(Keys key, KeyState keyState);

    void ClearAllKeys();

private:
    std::bitset<128> keyset;
};

} // namespace Pomdog

#endif // POMDOG_KEYBOARDSTATE_8518E1F7_HPP
