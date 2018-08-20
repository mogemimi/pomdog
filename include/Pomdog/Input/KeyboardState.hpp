// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include <bitset>

namespace Pomdog {

enum class KeyState : bool;
enum class Keys : std::uint8_t;

class POMDOG_EXPORT KeyboardState final {
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
