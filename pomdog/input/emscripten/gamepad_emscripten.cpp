// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/emscripten/gamepad_emscripten.h"
#include "pomdog/input/backends/gamepad_impl.h"
#include "pomdog/input/button_state.h"
#include "pomdog/input/gamepad_buttons.h"
#include "pomdog/input/player_index.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <algorithm>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::emscripten {
namespace {

void pollGamepad(GamepadImpl& impl, int gamepadIndex)
{
    EmscriptenGamepadEvent event;
    if (emscripten_get_gamepad_status(gamepadIndex, &event) != EMSCRIPTEN_RESULT_SUCCESS) {
        return;
    }

    if (event.connected == 0) {
        if (impl.isConnected()) {
            impl.disconnect();
        }
        return;
    }

    if (!impl.isConnected()) {
        impl.connect();
        impl.setName(event.id);
    }

    const int buttonCount = std::min(event.numButtons, 17);

    // NOTE: Standard gamepad button mapping
    constexpr GamepadButtons buttonMapping[] = {
        GamepadButtons::A,
        GamepadButtons::B,
        GamepadButtons::X,
        GamepadButtons::Y,
        GamepadButtons::LeftBumper,
        GamepadButtons::RightBumper,
        GamepadButtons::LeftTrigger,
        GamepadButtons::RightTrigger,
        GamepadButtons::LeftMenu,
        GamepadButtons::RightMenu,
        GamepadButtons::LeftStick,
        GamepadButtons::RightStick,
        GamepadButtons::DPadUp,
        GamepadButtons::DPadDown,
        GamepadButtons::DPadLeft,
        GamepadButtons::DPadRight,
        GamepadButtons::Guide,
    };

    for (int i = 0; i < buttonCount; ++i) {
        const auto isDown = (event.digitalButton[i] != 0);
        const auto buttonState = isDown ? ButtonState::Down : ButtonState::Up;
        const auto analogValue = static_cast<f32>(event.analogButton[i]);
        impl.setButtonState(buttonMapping[i], buttonState, std::clamp(analogValue, 0.0f, 1.0f));
    }

    // NOTE: Multiply vertical values by -1 (up = positive)
    constexpr double flip = -1.0;

    if (event.numAxes >= 2) {
        impl.setLeftStick(
            static_cast<f32>(event.axis[0]),
            static_cast<f32>(event.axis[1] * flip));
    }
    if (event.numAxes >= 4) {
        impl.setRightStick(
            static_cast<f32>(event.axis[2]),
            static_cast<f32>(event.axis[3] * flip));
    }
}

} // namespace

GamepadServiceEmscripten::GamepadServiceEmscripten() noexcept
{
    for (int i = 0; i < maxGamepads; ++i) {
        gamepads_[i] = std::make_shared<GamepadImpl>();
        gamepads_[i]->setPlayerIndex(static_cast<PlayerIndex>(i + 1));
    }
}

GamepadServiceEmscripten::~GamepadServiceEmscripten() noexcept = default;

std::shared_ptr<Gamepad>
GamepadServiceEmscripten::getGamepad(PlayerIndex playerIndex) noexcept
{
    const auto index = static_cast<int>(playerIndex) - 1;
    POMDOG_ASSERT(index >= 0 && index < maxGamepads);
    return gamepads_[index];
}

void GamepadServiceEmscripten::pollEvents() noexcept
{
    if (emscripten_sample_gamepad_data() != EMSCRIPTEN_RESULT_SUCCESS) {
        return;
    }

    for (int i = 0; i < maxGamepads; ++i) {
        pollGamepad(*gamepads_[i], i);
    }
}

} // namespace pomdog::detail::emscripten
