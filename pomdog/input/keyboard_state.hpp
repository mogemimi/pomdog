// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <bitset>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

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
    std::bitset<142> keyset;
};

} // namespace pomdog
