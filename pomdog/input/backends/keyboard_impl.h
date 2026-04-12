// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/input/backends/keyboard_state.h"
#include "pomdog/input/keyboard.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
enum class KeyState : u8;
enum class Keys : u8;
} // namespace pomdog

namespace pomdog::detail {

/// Platform-independent Keyboard implementation that holds the
/// current key states and text input. Platform backends forward
/// key events into this class via setKey() and appendTextInput().
class POMDOG_EXPORT KeyboardImpl final : public Keyboard {
private:
    KeyboardState keyboardState_;
    std::string textInput_;

public:
    KeyboardImpl();

    KeyboardImpl(const KeyboardImpl&) = delete;
    KeyboardImpl& operator=(const KeyboardImpl&) = delete;

    /// Determines whether the specified key is pressed.
    [[nodiscard]] bool
    isKeyDown(Keys key) const noexcept override;

    /// Determines whether the specified key is released.
    [[nodiscard]] bool
    isKeyUp(Keys key) const noexcept override;

    /// Return true if control key is down, false otherwise.
    [[nodiscard]] bool
    isControlKeyDown() const noexcept override;

    /// Return true if shift key is down, false otherwise.
    [[nodiscard]] bool
    isShiftKeyDown() const noexcept override;

    /// Return true if alt key is down, false otherwise.
    [[nodiscard]] bool
    isAltKeyDown() const noexcept override;

    /// Return true if super key is down, false otherwise.
    [[nodiscard]] bool
    isSuperKeyDown() const noexcept override;

    /// Returns true if any key is down, false otherwise.
    [[nodiscard]] bool
    isAnyKeyDown() const noexcept override;

    /// Returns the text input received during the current frame.
    [[nodiscard]] std::string_view
    getTextInput() const noexcept override;

    /// Sets the state of the specified key.
    void setKey(Keys key, KeyState keyState) noexcept;

    /// Appends text input received during the current frame.
    void appendTextInput(std::string_view text);

    /// Reset all key states to the unpressed state.
    void clearAllKeys() noexcept;

    /// Clear the text input buffer.
    void clearTextInput() noexcept;
};

} // namespace pomdog::detail
