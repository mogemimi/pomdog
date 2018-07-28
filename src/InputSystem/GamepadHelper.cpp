// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GamepadHelper.hpp"
#include "Pomdog/Input/GamepadState.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <algorithm>
#include <array>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace InputSystem {
namespace {

template <typename T>
T SwapEndian(T u)
{
    static_assert(sizeof(uint8_t) == 1, "");
    union {
        T u;
        uint8_t u8[sizeof(T)];
    } source, dest;
    source.u = u;
    for (size_t k = 0; k < sizeof(T); k++) {
        dest.u8[k] = source.u8[sizeof(T) - k - 1];
    }
    return dest.u;
}

} // namespace

std::string GamepadHelper::ToString(const GamepadDeviceID& device)
{
    std::array<uint16_t, 8> uuid;
    std::fill(std::begin(uuid), std::end(uuid), static_cast<uint16_t>(0));
    if ((device.Vendor != 0) && (device.Product != 0)) {
        uuid[0] = device.BusType;
        uuid[1] = 0;
        uuid[2] = device.Vendor;
        uuid[3] = 0;
        uuid[4] = device.Product;
        uuid[5] = 0;
        uuid[6] = device.Version;
        uuid[7] = 0;
    }

    std::string s;
    for (auto u : uuid) {
        s += StringHelper::Format("%04x", SwapEndian(u));
    }
    return s;
}

int GamepadHelper::ToInt(PlayerIndex index)
{
    POMDOG_ASSERT(static_cast<int>(index) >= 1);
    return static_cast<int>(index) - 1;
}

void GamepadHelper::ClearState(GamepadState& state)
{
    state.Buttons.A = ButtonState::Released;
    state.Buttons.B = ButtonState::Released;
    state.Buttons.X = ButtonState::Released;
    state.Buttons.Y = ButtonState::Released;
    state.Buttons.LeftShoulder = ButtonState::Released;
    state.Buttons.RightShoulder = ButtonState::Released;
    state.Buttons.LeftTrigger = ButtonState::Released;
    state.Buttons.RightTrigger = ButtonState::Released;
    state.Buttons.LeftMenu = ButtonState::Released;
    state.Buttons.RightMenu = ButtonState::Released;
    state.Buttons.LeftStick = ButtonState::Released;
    state.Buttons.RightStick = ButtonState::Released;
    state.Buttons.Guide = ButtonState::Released;
    state.Buttons.Extra1 = ButtonState::Released;
    state.Buttons.Extra2 = ButtonState::Released;
    state.DPad.Down = ButtonState::Released;
    state.DPad.Up = ButtonState::Released;
    state.DPad.Left = ButtonState::Released;
    state.DPad.Right = ButtonState::Released;
    state.ThumbSticks.Left = Vector2::Zero;
    state.ThumbSticks.Right = Vector2::Zero;
    state.IsConnected = false;
}

} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog
