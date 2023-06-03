// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <bitset>
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

enum class KeyState : bool;
enum class Keys : std::uint8_t;

class POMDOG_EXPORT KeyboardState final {
private:
    std::bitset<142> keyset_;

public:
    [[nodiscard]] bool
    isKeyDown(Keys key) const noexcept;

    [[nodiscard]] bool
    isKeyUp(Keys key) const noexcept;

    void setKey(Keys key, KeyState keyState) noexcept;

    void clearAllKeys() noexcept;
};

} // namespace pomdog
