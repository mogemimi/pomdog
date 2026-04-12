// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Vector2;
struct GamepadCapabilities;
enum class PlayerIndex : i8;
enum class GamepadButtons : i8;
} // namespace pomdog

namespace pomdog {

/// Represents a connected gamepad device.
class POMDOG_EXPORT Gamepad {
public:
    Gamepad() noexcept;

    Gamepad(const Gamepad&) = delete;
    Gamepad& operator=(const Gamepad&) = delete;

    virtual ~Gamepad();

    /// Returns a name of the gamepad as provided by the driver.
    [[nodiscard]] virtual std::string_view
    getName() const noexcept = 0;

    /// Gets the player index of this gamepad.
    [[nodiscard]] virtual PlayerIndex
    getPlayerIndex() const noexcept = 0;

    /// Returns true if the gamepad is connected, false otherwise.
    [[nodiscard]] virtual bool
    isConnected() const noexcept = 0;

    /// Determines whether the specified gamepad button is pressed.
    [[nodiscard]] virtual bool
    isButtonDown(GamepadButtons button) const noexcept = 0;

    /// Gets the analog value from the specified button (0.0 to 1.0).
    [[nodiscard]] virtual f32
    getAnalogButton(GamepadButtons button) const noexcept = 0;

    /// Returns true if any button is pressed, false otherwise.
    [[nodiscard]] virtual bool
    isAnyButtonDown() const noexcept = 0;

    /// Gets the left thumbstick. Negative values indicate down or left.
    /// Positive values indicate up or right.
    [[nodiscard]] virtual Vector2
    getLeftStick() const noexcept = 0;

    /// Gets the right thumbstick. Negative values indicate down or left.
    /// Positive values indicate up or right.
    [[nodiscard]] virtual Vector2
    getRightStick() const noexcept = 0;

    /// Gets the capabilities of this gamepad.
    [[nodiscard]] virtual const GamepadCapabilities&
    getCapabilities() const noexcept = 0;
};

} // namespace pomdog
