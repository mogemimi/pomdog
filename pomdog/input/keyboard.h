// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
enum class Keys : u8;
} // namespace pomdog

namespace pomdog {

class POMDOG_EXPORT Keyboard {
public:
    Keyboard() noexcept;

    Keyboard(const Keyboard&) = delete;
    Keyboard& operator=(const Keyboard&) = delete;

    virtual ~Keyboard();

    /// Determines whether the specified key is pressed.
    [[nodiscard]] virtual bool
    isKeyDown(Keys key) const noexcept = 0;

    /// Determines whether the specified key is released.
    [[nodiscard]] virtual bool
    isKeyUp(Keys key) const noexcept = 0;

    /// Return true if control key is down, false otherwise.
    [[nodiscard]] virtual bool
    isControlKeyDown() const noexcept = 0;

    /// Return true if shift key is down, false otherwise.
    [[nodiscard]] virtual bool
    isShiftKeyDown() const noexcept = 0;

    /// Return true if alt key is down, false otherwise.
    [[nodiscard]] virtual bool
    isAltKeyDown() const noexcept = 0;

    /// Return true if super key is down, false otherwise.
    [[nodiscard]] virtual bool
    isSuperKeyDown() const noexcept = 0;

    /// Returns true if any key is down, false otherwise.
    [[nodiscard]] virtual bool
    isAnyKeyDown() const noexcept = 0;

    /// Returns the text input received during the current frame.
    [[nodiscard]] virtual std::string_view
    getTextInput() const noexcept = 0;
};

} // namespace pomdog
