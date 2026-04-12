// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/input/gamepad.h"
#include "pomdog/input/gamepad_capabilities.h"
#include "pomdog/input/player_index.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <bitset>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
enum class ButtonState : u8;
} // namespace pomdog

namespace pomdog::detail {
inline constexpr i32 GamepadButtonsCount = 19;
} // namespace pomdog::detail

namespace pomdog::detail {

/// Platform-independent Gamepad implementation that holds the
/// current button, analog, and stick state. Platform backends
/// write to this class (or a back-buffer copy) via the setters.
class GamepadImpl final : public Gamepad {
private:
    GamepadCapabilities capabilities_;
    std::string name_ = {};
    std::array<f32, GamepadButtonsCount + 1> analogButtons_ = {};
    Vector2 leftStick_ = Vector2::createZero();
    Vector2 rightStick_ = Vector2::createZero();
    std::bitset<GamepadButtonsCount + 1> buttons_ = {};
    PlayerIndex playerIndex_ = PlayerIndex::One;
    bool connected_ = false;

public:
    GamepadImpl();

    GamepadImpl(const GamepadImpl&) = delete;
    GamepadImpl& operator=(const GamepadImpl&) = delete;

    [[nodiscard]] std::string_view
    getName() const noexcept override;

    [[nodiscard]] PlayerIndex
    getPlayerIndex() const noexcept override;

    [[nodiscard]] bool
    isConnected() const noexcept override;

    [[nodiscard]] bool
    isButtonDown(GamepadButtons button) const noexcept override;

    [[nodiscard]] f32
    getAnalogButton(GamepadButtons button) const noexcept override;

    [[nodiscard]] bool
    isAnyButtonDown() const noexcept override;

    [[nodiscard]] Vector2
    getLeftStick() const noexcept override;

    [[nodiscard]] Vector2
    getRightStick() const noexcept override;

    [[nodiscard]] const GamepadCapabilities&
    getCapabilities() const noexcept override;

    /// Sets the pressed/released and analog value for a button.
    void setButtonState(GamepadButtons button, ButtonState buttonState, f32 analogValue) noexcept;

    /// Sets the left thumbstick position.
    void setLeftStick(f32 x, f32 y) noexcept;

    /// Sets the right thumbstick position.
    void setRightStick(f32 x, f32 y) noexcept;

    /// Sets the left thumbstick X axis.
    void setLeftStickX(f32 x) noexcept;

    /// Sets the left thumbstick Y axis.
    void setLeftStickY(f32 y) noexcept;

    /// Sets the right thumbstick X axis.
    void setRightStickX(f32 x) noexcept;

    /// Sets the right thumbstick Y axis.
    void setRightStickY(f32 y) noexcept;

    /// Sets the device name.
    void setName(std::string name) noexcept;

    /// Sets the device capabilities.
    void setCapabilities(GamepadCapabilities caps) noexcept;

    /// Sets the player index.
    void setPlayerIndex(PlayerIndex playerIndex) noexcept;

    /// Marks this gamepad as connected.
    void connect() noexcept;

    /// Marks this gamepad as disconnected and resets all state.
    void disconnect() noexcept;

    /// Copies per-frame state (buttons, sticks) from another instance.
    void copyStateFrom(const GamepadImpl& other) noexcept;
};

} // namespace pomdog::detail
