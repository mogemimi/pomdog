// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GamepadMappings.hpp"
#include "../Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Platform.hpp"
#include "Pomdog/Input/GamepadState.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <algorithm>
#include <array>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace InputSystem {
namespace {

#ifdef POMDOG_PLATFORM_MACOSX
#ifndef __MACOSX__
#define __MACOSX__ 1
#endif
#endif

#ifdef POMDOG_PLATFORM_LINUX
#ifndef __LINUX__
#define __LINUX__ 1
#endif
#endif
#include "SDL_gamecontrollerdb.h"

std::tuple<std::string, const char*> Parse(const char* source, char delimiter)
{
    std::string identifier;
    while (*source != 0) {
        if (*source == delimiter) {
            ++source;
            break;
        }
        identifier += *source;
        ++source;
    }
    return std::make_tuple(identifier, source);
}

void ParseMapping(const char* source, GamepadMappings& mappings, std::string& name)
{
    std::string uuid;
    std::string key;
    std::string index;

    std::tie(uuid, source) = Parse(source, ',');
    std::tie(name, source) = Parse(source, ',');

    const std::array<ButtonKind, 21> buttons = {{
        ButtonKind::A,
        ButtonKind::B,
        ButtonKind::LeftMenu,
        ButtonKind::None, // dpdown
        ButtonKind::None, // dpleft
        ButtonKind::None, // dpright
        ButtonKind::None, // dpup
        ButtonKind::Guide,
        ButtonKind::LeftShoulder,
        ButtonKind::LeftStick,
        ButtonKind::LeftTrigger,
        ButtonKind::None, // leftx
        ButtonKind::None, // lefty
        ButtonKind::RightShoulder,
        ButtonKind::RightStick,
        ButtonKind::RightTrigger,
        ButtonKind::None, // rightx
        ButtonKind::None, // righty
        ButtonKind::RightMenu,
        ButtonKind::X,
        ButtonKind::Y,
    }};

    const std::array<AxesKind, 21> axes = {{
        AxesKind::None, // A
        AxesKind::None, // B
        AxesKind::None, // LeftMenu
        AxesKind::None, // dpdown
        AxesKind::None, // dpleft
        AxesKind::None, // dpright
        AxesKind::None, // dpup
        AxesKind::None, // Guide
        AxesKind::None, // LeftShoulder
        AxesKind::None, // LeftStick
        AxesKind::LeftTrigger,
        AxesKind::LeftStickX,
        AxesKind::LeftStickY,
        AxesKind::None, // RightShoulder
        AxesKind::None, // RightStick
        AxesKind::RightTrigger,
        AxesKind::RightStickX,
        AxesKind::RightStickY,
        AxesKind::None, // RightMenu
        AxesKind::None, // X
        AxesKind::None, // Y
    }};

    int buttonIndex = 0;
    while (*source != 0) {
        std::tie(key, source) = Parse(source, ':');
        std::tie(index, source) = Parse(source, ',');

        Log::Internal(key + " " + index);

        if (index[0] == 'b') {
            int i = std::atoi(StringHelper::TrimLeft(index, 'b').c_str());
            if ((i >= 0) && (i < static_cast<int>(mappings.buttons.size()))) {
                mappings.buttons[i] = buttons[buttonIndex];
            }
        }
        else if (index[0] == 'a') {
            auto s = StringHelper::TrimRight(index, '~');
            s = StringHelper::TrimLeft(s, '+');
            s = StringHelper::TrimLeft(s, '-');
            s = StringHelper::TrimLeft(s, 'a');
            int i = std::atoi(s.c_str());
            if ((i >= 0) && (i < static_cast<int>(mappings.axes.size())))  {
                mappings.axes[i] = axes[buttonIndex];
            }
        }
        ++buttonIndex;
    }
}

ButtonKind ToButtonIndex(int physicalIndex, GamepadButtonMappings mappings)
{
    if ((physicalIndex < 0) || (physicalIndex >= static_cast<int>(mappings.size()))) {
        return ButtonKind::None;
    }
    POMDOG_ASSERT(physicalIndex >= 0);
    POMDOG_ASSERT(physicalIndex < static_cast<int>(mappings.size()));
    return mappings[physicalIndex];
}

} // namespace

ButtonState* GetButton(GamepadState& state, const GamepadButtonMappings& mappings, int physicalIndex)
{
    const auto buttonKind = ToButtonIndex(physicalIndex, mappings);
    if (buttonKind == ButtonKind::None) {
        return nullptr;
    }
    const auto index = static_cast<int>(buttonKind);

    std::array<ButtonState*, 15> buttons = {{
        &state.Buttons.A,
        &state.Buttons.B,
        &state.Buttons.X,
        &state.Buttons.Y,
        &state.Buttons.LeftShoulder,
        &state.Buttons.RightShoulder,
        &state.Buttons.LeftTrigger,
        &state.Buttons.RightTrigger,
        &state.Buttons.LeftMenu,
        &state.Buttons.RightMenu,
        &state.Buttons.LeftStick,
        &state.Buttons.RightStick,
        &state.Buttons.Guide,
        &state.Buttons.Extra1,
        &state.Buttons.Extra2,
    }};
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(buttons.size()));
    return buttons[index];
}

std::tuple<GamepadMappings, std::string> GetMappings(const GamepadDeviceID& uuid)
{
    auto uuidString = GamepadHelper::ToString(uuid);

    Log::Internal("UUID: " + uuidString);

    GamepadMappings mappings;
#ifdef POMDOG_PLATFORM_LINUX
    // NOTE: Please see this header
    // https://github.com/torvalds/linux/blob/4982327ff6755377a8a66e84113f496f3a6c53bc/include/uapi/linux/input-event-codes.h#L379-L398
    mappings.buttons = {{
        ButtonKind::A,
        ButtonKind::B,
        ButtonKind::X,
        ButtonKind::Y,
        ButtonKind::LeftShoulder,
        ButtonKind::RightShoulder,
        ButtonKind::LeftMenu,
        ButtonKind::RightMenu,
        ButtonKind::Guide,
        ButtonKind::LeftStick,
        ButtonKind::RightStick,
        ButtonKind::Extra1,
        ButtonKind::Extra2,
        ButtonKind::None,
        ButtonKind::None,
        ButtonKind::None,
    }};
#else
    // POMDOG_PLATFORM_MACOSX
    mappings.buttons = {{
        ButtonKind::A,
        ButtonKind::B,
        ButtonKind::X,
        ButtonKind::Y,
        ButtonKind::LeftShoulder,
        ButtonKind::RightShoulder,
        ButtonKind::LeftTrigger,
        ButtonKind::RightTrigger,
        ButtonKind::LeftMenu,
        ButtonKind::RightMenu,
        ButtonKind::LeftStick,
        ButtonKind::RightStick,
        ButtonKind::Guide,
        ButtonKind::Extra1,
        ButtonKind::Extra2,
        ButtonKind::None,
    }};
#endif

    mappings.axes = {{
        AxesKind::LeftStickX,
        AxesKind::LeftStickY,
        AxesKind::LeftTrigger,
        AxesKind::RightStickX,
        AxesKind::RightStickY,
        AxesKind::RightTrigger,
    }};

    std::string deviceName;
    for (auto m : s_ControllerMappings) {
        if (m == nullptr) {
            break;
        }
        std::string s = m;
        if (StringHelper::HasPrefix(s, uuidString)) {
            // found
            std::fill(std::begin(mappings.buttons), std::end(mappings.buttons), ButtonKind::None);
            std::fill(std::begin(mappings.axes), std::end(mappings.axes), AxesKind::None);
            ParseMapping(m, mappings, deviceName);
            break;
        }
    }
    return std::make_tuple(mappings, deviceName);
}

} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog
