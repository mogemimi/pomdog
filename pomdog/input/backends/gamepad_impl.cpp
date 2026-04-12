// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/backends/gamepad_impl.h"
#include "pomdog/input/button_state.h"
#include "pomdog/input/gamepad_buttons.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

GamepadImpl::GamepadImpl()
{
    analogButtons_.fill(0.0f);
}

std::string_view
GamepadImpl::getName() const noexcept
{
    return name_;
}

PlayerIndex
GamepadImpl::getPlayerIndex() const noexcept
{
    return playerIndex_;
}

bool GamepadImpl::isConnected() const noexcept
{
    return connected_;
}

bool GamepadImpl::isButtonDown(GamepadButtons button) const noexcept
{
    static_assert(static_cast<i32>(GamepadButtons::Invalid) == -1);
    const auto index = static_cast<u32>(static_cast<i32>(button) + 1);
    POMDOG_ASSERT(index < buttons_.size());
    return buttons_.test(index);
}

f32 GamepadImpl::getAnalogButton(GamepadButtons button) const noexcept
{
    static_assert(static_cast<i32>(GamepadButtons::Invalid) == -1);
    const auto index = static_cast<u32>(static_cast<i32>(button) + 1);
    POMDOG_ASSERT(index < analogButtons_.size());
    return analogButtons_[index];
}

bool GamepadImpl::isAnyButtonDown() const noexcept
{
    return buttons_.any();
}

Vector2
GamepadImpl::getLeftStick() const noexcept
{
    return leftStick_;
}

Vector2
GamepadImpl::getRightStick() const noexcept
{
    return rightStick_;
}

const GamepadCapabilities&
GamepadImpl::getCapabilities() const noexcept
{
    return capabilities_;
}

void GamepadImpl::setButtonState(GamepadButtons button, ButtonState buttonState, f32 analogValue) noexcept
{
    static_assert(static_cast<i32>(GamepadButtons::Invalid) == -1);
    const auto index = static_cast<u32>(static_cast<i32>(button) + 1);
    POMDOG_ASSERT(index < buttons_.size());
    POMDOG_ASSERT(index < analogButtons_.size());
    POMDOG_ASSERT(analogValue >= 0.0f);
    POMDOG_ASSERT(analogValue <= 1.0f);

    buttons_.set(index, (buttonState == ButtonState::Down));
    analogButtons_[index] = analogValue;
}

void GamepadImpl::setLeftStick(f32 x, f32 y) noexcept
{
    leftStick_ = Vector2{x, y};
}

void GamepadImpl::setRightStick(f32 x, f32 y) noexcept
{
    rightStick_ = Vector2{x, y};
}

void GamepadImpl::setLeftStickX(f32 x) noexcept
{
    leftStick_.x = x;
}

void GamepadImpl::setLeftStickY(f32 y) noexcept
{
    leftStick_.y = y;
}

void GamepadImpl::setRightStickX(f32 x) noexcept
{
    rightStick_.x = x;
}

void GamepadImpl::setRightStickY(f32 y) noexcept
{
    rightStick_.y = y;
}

void GamepadImpl::setName(std::string name) noexcept
{
    name_ = std::move(name);
}

void GamepadImpl::setCapabilities(GamepadCapabilities caps) noexcept
{
    capabilities_ = std::move(caps);
}

void GamepadImpl::setPlayerIndex(PlayerIndex playerIndex) noexcept
{
    playerIndex_ = playerIndex;
}

void GamepadImpl::connect() noexcept
{
    connected_ = true;

    buttons_.reset();
    std::fill(analogButtons_.begin(), analogButtons_.end(), 0.0f);
    leftStick_ = Vector2::createZero();
    rightStick_ = Vector2::createZero();
}

void GamepadImpl::disconnect() noexcept
{
    connected_ = false;

    buttons_.reset();
    std::fill(analogButtons_.begin(), analogButtons_.end(), 0.0f);
    leftStick_ = Vector2::createZero();
    rightStick_ = Vector2::createZero();
}

void GamepadImpl::copyStateFrom(const GamepadImpl& other) noexcept
{
    buttons_ = other.buttons_;
    analogButtons_ = other.analogButtons_;
    leftStick_ = other.leftStick_;
    rightStick_ = other.rightStick_;
}

} // namespace pomdog::detail
