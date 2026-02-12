// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <bitset>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
enum class KeyState : u8;
enum class Keys : u8;
} // namespace pomdog

namespace pomdog {

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
