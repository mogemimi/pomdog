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
#include <unordered_map>
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

#ifdef POMDOG_PLATFORM_WIN32
#ifndef SDL_JOYSTICK_XINPUT
#define SDL_JOYSTICK_XINPUT 1
#endif
#ifndef SDL_JOYSTICK_DINPUT
#define SDL_JOYSTICK_DINPUT 1
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

    const std::unordered_map<std::string, std::tuple<ButtonKind, ThumbStickKind>> dict = {
        {"a", std::make_tuple(ButtonKind::A, ThumbStickKind::None)},
        {"b", std::make_tuple(ButtonKind::B, ThumbStickKind::None)},
        {"x", std::make_tuple(ButtonKind::X, ThumbStickKind::None)},
        {"y", std::make_tuple(ButtonKind::Y, ThumbStickKind::None)},
        {"guide", std::make_tuple(ButtonKind::Guide, ThumbStickKind::None)},
        {"back", std::make_tuple(ButtonKind::LeftMenu, ThumbStickKind::None)},
        {"start", std::make_tuple(ButtonKind::RightMenu, ThumbStickKind::None)},
        {"leftshoulder", std::make_tuple(ButtonKind::LeftShoulder, ThumbStickKind::None)},
        {"rightshoulder", std::make_tuple(ButtonKind::RightShoulder, ThumbStickKind::None)},
        {"leftstick", std::make_tuple(ButtonKind::LeftStick, ThumbStickKind::None)},
        {"rightstick", std::make_tuple(ButtonKind::RightStick, ThumbStickKind::None)},
        {"dpup", std::make_tuple(ButtonKind::DPadUp, ThumbStickKind::None)},
        {"dpdown", std::make_tuple(ButtonKind::DPadDown, ThumbStickKind::None)},
        {"dpleft", std::make_tuple(ButtonKind::DPadLeft, ThumbStickKind::None)},
        {"dpright", std::make_tuple(ButtonKind::DPadRight, ThumbStickKind::None)},
        {"lefttrigger", std::make_tuple(ButtonKind::LeftTrigger, ThumbStickKind::None)},
        {"righttrigger", std::make_tuple(ButtonKind::RightTrigger, ThumbStickKind::None)},
        {"leftx", std::make_tuple(ButtonKind::None, ThumbStickKind::LeftStickX)},
        {"lefty", std::make_tuple(ButtonKind::None, ThumbStickKind::LeftStickY)},
        {"rightx", std::make_tuple(ButtonKind::None, ThumbStickKind::RightStickX)},
        {"righty", std::make_tuple(ButtonKind::None, ThumbStickKind::RightStickY)},
    };

    int buttonIndex = 0;
    while (*source != 0) {
        std::tie(key, source) = Parse(source, ':');
        std::tie(index, source) = Parse(source, ',');

        auto kind = dict.find(key);
        if (kind == std::end(dict)) {
            ++buttonIndex;
            continue;
        }

        if (index[0] == 'b') {
            int i = std::atoi(StringHelper::TrimLeft(index, 'b').c_str());
            if ((i >= 0) && (i < static_cast<int>(mappings.buttons.size()))) {
                mappings.buttons[i] = std::get<ButtonKind>(kind->second);
            }
        }
        else if (index[0] == 'a') {
            auto s = StringHelper::TrimRight(index, '~');
            s = StringHelper::TrimLeft(s, 'a');
            int i = std::atoi(s.c_str());
            if ((i >= 0) && (i < static_cast<int>(mappings.axes.size()))) {
                mappings.axes[i].thumbStick = std::get<ThumbStickKind>(kind->second);
                mappings.axes[i].positiveTrigger = std::get<ButtonKind>(kind->second);
            }
        }
        else if (StringHelper::HasPrefix(index, "+a")) {
            auto s = StringHelper::TrimRight(index, '~');
            s = StringHelper::TrimLeft(s, '+');
            s = StringHelper::TrimLeft(s, 'a');
            int i = std::atoi(s.c_str());
            if ((i >= 0) && (i < static_cast<int>(mappings.axes.size()))) {
                mappings.axes[i].positiveTrigger = std::get<ButtonKind>(kind->second);
            }
        }
        else if (StringHelper::HasPrefix(index, "-a")) {
            auto s = StringHelper::TrimRight(index, '~');
            s = StringHelper::TrimLeft(s, '-');
            s = StringHelper::TrimLeft(s, 'a');
            int i = std::atoi(s.c_str());
            if ((i >= 0) && (i < static_cast<int>(mappings.axes.size()))) {
                mappings.axes[i].negativeTrigger = std::get<ButtonKind>(kind->second);
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
    const auto kind = ToButtonIndex(physicalIndex, mappings);
    return GetButton(state, kind);
}

ButtonState* GetButton(GamepadState& state, ButtonKind kind)
{
    if (kind == ButtonKind::None) {
        return nullptr;
    }
    const auto index = static_cast<int>(kind);

    std::array<ButtonState*, 19> buttons = {{
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
        &state.DPad.Up,
        &state.DPad.Down,
        &state.DPad.Left,
        &state.DPad.Right,
    }};
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(buttons.size()));
    return buttons[index];
}

float* GetThumbStick(GamepadState& state, ThumbStickKind kind)
{
    if (kind == ThumbStickKind::None) {
        return nullptr;
    }
    const auto index = static_cast<int>(kind);

    std::array<float*, 4> axes = {{
        &state.ThumbSticks.Left.X,
        &state.ThumbSticks.Left.Y,
        &state.ThumbSticks.Right.X,
        &state.ThumbSticks.Right.Y,
    }};
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(axes.size()));
    return axes[index];
}

bool* HasButton(GamepadCapabilities& caps, const GamepadButtonMappings& mappings, int physicalIndex)
{
    const auto kind = ToButtonIndex(physicalIndex, mappings);
    return HasButton(caps, kind);
}

bool* HasButton(GamepadCapabilities& caps, ButtonKind kind)
{
    if (kind == ButtonKind::None) {
        return nullptr;
    }
    const auto index = static_cast<int>(kind);

    std::array<bool*, 19> buttons = {{
        &caps.HasAButton,
        &caps.HasBButton,
        &caps.HasXButton,
        &caps.HasYButton,
        &caps.HasLeftShoulderButton,
        &caps.HasRightShoulderButton,
        &caps.HasLeftTrigger,
        &caps.HasRightTrigger,
        &caps.HasLeftMenuButton,
        &caps.HasRightMenuButton,
        &caps.HasLeftStickButton,
        &caps.HasRightStickButton,
        &caps.HasGuideButton,
        &caps.HasExtra1Button,
        &caps.HasExtra2Button,
        nullptr, // DPad.Up
        nullptr, // DPad.Down
        nullptr, // DPad.Left
        nullptr, // DPad.Right
    }};
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(buttons.size()));
    return buttons[index];
}

bool* HasThumbStick(GamepadCapabilities& caps, ThumbStickKind kind)
{
    if (kind == ThumbStickKind::None) {
        return nullptr;
    }
    const auto index = static_cast<int>(kind);

    std::array<bool*, 4> axes = {{
        &caps.HasLeftXThumbStick,
        &caps.HasLeftYThumbStick,
        &caps.HasRightXThumbStick,
        &caps.HasRightYThumbStick,
    }};
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(axes.size()));
    return axes[index];
}

std::tuple<GamepadMappings, std::string> GetMappings(const GamepadUUID& uuid)
{
    return GetMappings(uuid.ToString());
}

std::tuple<GamepadMappings, std::string> GetMappings(const std::string& uuidString)
{
    GamepadMappings mappings;
#if defined(POMDOG_PLATFORM_LINUX)
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
#elif defined(POMDOG_PLATFORM_MACOSX)
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
#else
    mappings.buttons = {{
        ButtonKind::A,
        ButtonKind::B,
        ButtonKind::X,
        ButtonKind::Y,
        ButtonKind::LeftShoulder,
        ButtonKind::RightShoulder,
        ButtonKind::LeftMenu,
        ButtonKind::RightMenu,
        ButtonKind::LeftStick,
        ButtonKind::RightStick,
        ButtonKind::Guide,
        ButtonKind::Extra1,
        ButtonKind::Extra2,
        ButtonKind::None,
        ButtonKind::None,
        ButtonKind::None,
    }};
#endif

    mappings.axes = {{
        AxisMapper{ ThumbStickKind::LeftStickX, ButtonKind::None, ButtonKind::None },
        AxisMapper{ ThumbStickKind::LeftStickY, ButtonKind::None, ButtonKind::None },
        AxisMapper{ ThumbStickKind::None, ButtonKind::LeftTrigger, ButtonKind::None },
        AxisMapper{ ThumbStickKind::RightStickX, ButtonKind::None, ButtonKind::None },
        AxisMapper{ ThumbStickKind::RightStickY, ButtonKind::None, ButtonKind::None },
        AxisMapper{ ThumbStickKind::None, ButtonKind::RightTrigger, ButtonKind::None },
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
            for (auto& axis : mappings.axes) {
                axis.thumbStick = ThumbStickKind::None;
                axis.positiveTrigger = ButtonKind::None;
                axis.negativeTrigger = ButtonKind::None;
            }
            ParseMapping(m, mappings, deviceName);
            break;
        }
    }
    return std::make_tuple(mappings, deviceName);
}

} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog
