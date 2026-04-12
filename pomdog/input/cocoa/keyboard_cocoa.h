// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/backends/keyboard_state.h"
#include "pomdog/input/keyboard.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
class SystemEvent;
} // namespace pomdog::detail

namespace pomdog::detail::cocoa {

class KeyboardCocoa final : public Keyboard {
private:
    KeyboardState state_;
    std::string textInput_;

public:
    KeyboardCocoa();

    [[nodiscard]] bool
    isKeyDown(Keys key) const noexcept override;

    [[nodiscard]] bool
    isKeyUp(Keys key) const noexcept override;

    [[nodiscard]] bool
    isControlKeyDown() const noexcept override;

    [[nodiscard]] bool
    isShiftKeyDown() const noexcept override;

    [[nodiscard]] bool
    isAltKeyDown() const noexcept override;

    [[nodiscard]] bool
    isSuperKeyDown() const noexcept override;

    [[nodiscard]] bool
    isAnyKeyDown() const noexcept override;

    [[nodiscard]] std::string_view
    getTextInput() const noexcept override;

    void handleEvent(const SystemEvent& event);

    void clearAllKeys() noexcept;

    void clearTextInput() noexcept;
};

} // namespace pomdog::detail::cocoa
