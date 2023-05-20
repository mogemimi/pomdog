// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/backends/gamepad_mappings.h"
#include "pomdog/basic/compilation_targets.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/input/gamepad_state.h"
#include "pomdog/logging/log.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <charconv>
#include <unordered_map>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
namespace {

#include "SDL_GameControllerDB.h"

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
        {"a", std::make_tuple(ButtonKind::A, ThumbStickKind::Invalid)},
        {"b", std::make_tuple(ButtonKind::B, ThumbStickKind::Invalid)},
        {"x", std::make_tuple(ButtonKind::X, ThumbStickKind::Invalid)},
        {"y", std::make_tuple(ButtonKind::Y, ThumbStickKind::Invalid)},
        {"guide", std::make_tuple(ButtonKind::Guide, ThumbStickKind::Invalid)},
        {"back", std::make_tuple(ButtonKind::LeftMenu, ThumbStickKind::Invalid)},
        {"start", std::make_tuple(ButtonKind::RightMenu, ThumbStickKind::Invalid)},
        {"leftshoulder", std::make_tuple(ButtonKind::LeftShoulder, ThumbStickKind::Invalid)},
        {"rightshoulder", std::make_tuple(ButtonKind::RightShoulder, ThumbStickKind::Invalid)},
        {"leftstick", std::make_tuple(ButtonKind::LeftStick, ThumbStickKind::Invalid)},
        {"rightstick", std::make_tuple(ButtonKind::RightStick, ThumbStickKind::Invalid)},
        {"dpup", std::make_tuple(ButtonKind::DPadUp, ThumbStickKind::Invalid)},
        {"dpdown", std::make_tuple(ButtonKind::DPadDown, ThumbStickKind::Invalid)},
        {"dpleft", std::make_tuple(ButtonKind::DPadLeft, ThumbStickKind::Invalid)},
        {"dpright", std::make_tuple(ButtonKind::DPadRight, ThumbStickKind::Invalid)},
        {"lefttrigger", std::make_tuple(ButtonKind::LeftTrigger, ThumbStickKind::Invalid)},
        {"righttrigger", std::make_tuple(ButtonKind::RightTrigger, ThumbStickKind::Invalid)},
        {"leftx", std::make_tuple(ButtonKind::Invalid, ThumbStickKind::LeftStickX)},
        {"lefty", std::make_tuple(ButtonKind::Invalid, ThumbStickKind::LeftStickY)},
        {"rightx", std::make_tuple(ButtonKind::Invalid, ThumbStickKind::RightStickX)},
        {"righty", std::make_tuple(ButtonKind::Invalid, ThumbStickKind::RightStickY)},
    };

    while (*source != 0) {
        std::tie(key, source) = Parse(source, ':');
        std::tie(index, source) = Parse(source, ',');

        auto kind = dict.find(key);
        if (kind == std::end(dict)) {
            continue;
        }

        if (index[0] == 'b') {
            auto s = StringHelper::TrimLeft(index, 'b');
            int i = 0;
            if (auto [p, err] = std::from_chars(s.data(), s.data() + s.size(), i); err != std::errc{}) {
                i = -1;
            }
            if ((i >= 0) && (i < static_cast<int>(mappings.buttons.size()))) {
                mappings.buttons[i] = std::get<ButtonKind>(kind->second);
            }
        }
        else if (index[0] == 'a') {
            auto s = StringHelper::TrimRight(index, '~');
            s = StringHelper::TrimLeft(s, 'a');
            int i = 0;
            if (auto [p, err] = std::from_chars(s.data(), s.data() + s.size(), i); err != std::errc{}) {
                i = -1;
            }
            if ((i >= 0) && (i < static_cast<int>(mappings.axes.size()))) {
                mappings.axes[i].thumbStick = std::get<ThumbStickKind>(kind->second);
                mappings.axes[i].positiveTrigger = std::get<ButtonKind>(kind->second);
            }
        }
        else if (StringHelper::HasPrefix(index, "+a")) {
            auto s = StringHelper::TrimRight(index, '~');
            s = StringHelper::TrimLeft(s, '+');
            s = StringHelper::TrimLeft(s, 'a');
            int i = 0;
            if (auto [p, err] = std::from_chars(s.data(), s.data() + s.size(), i); err != std::errc{}) {
                i = -1;
            }
            if ((i >= 0) && (i < static_cast<int>(mappings.axes.size()))) {
                mappings.axes[i].positiveTrigger = std::get<ButtonKind>(kind->second);
            }
        }
        else if (StringHelper::HasPrefix(index, "-a")) {
            auto s = StringHelper::TrimRight(index, '~');
            s = StringHelper::TrimLeft(s, '-');
            s = StringHelper::TrimLeft(s, 'a');
            int i = 0;
            if (auto [p, err] = std::from_chars(s.data(), s.data() + s.size(), i); err != std::errc{}) {
                i = -1;
            }
            if ((i >= 0) && (i < static_cast<int>(mappings.axes.size()))) {
                mappings.axes[i].negativeTrigger = std::get<ButtonKind>(kind->second);
            }
        }
    }
}

ButtonKind ToButtonIndex(int physicalIndex, GamepadButtonMappings mappings)
{
    if ((physicalIndex < 0) || (physicalIndex >= static_cast<int>(mappings.size()))) {
        return ButtonKind::Invalid;
    }
    POMDOG_ASSERT(physicalIndex >= 0);
    POMDOG_ASSERT(physicalIndex < static_cast<int>(mappings.size()));
    return mappings[physicalIndex];
}

} // namespace

[[nodiscard]] ButtonState*
GetButton(GamepadState& state, const GamepadButtonMappings& mappings, int physicalIndex) noexcept
{
    const auto kind = ToButtonIndex(physicalIndex, mappings);
    return GetButton(state, kind);
}

[[nodiscard]] ButtonState*
GetButton(GamepadState& state, ButtonKind kind) noexcept
{
    if (kind == ButtonKind::Invalid) {
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

[[nodiscard]] float*
GetThumbStick(GamepadState& state, ThumbStickKind kind) noexcept
{
    if (kind == ThumbStickKind::Invalid) {
        return nullptr;
    }
    const auto index = static_cast<int>(kind);

    std::array<float*, 4> axes = {{
        &state.ThumbSticks.Left.x,
        &state.ThumbSticks.Left.y,
        &state.ThumbSticks.Right.x,
        &state.ThumbSticks.Right.y,
    }};
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(axes.size()));
    return axes[index];
}

[[nodiscard]] bool*
HasButton(GamepadCapabilities& caps, const GamepadButtonMappings& mappings, int physicalIndex) noexcept
{
    const auto kind = ToButtonIndex(physicalIndex, mappings);
    return HasButton(caps, kind);
}

[[nodiscard]] bool*
HasButton(GamepadCapabilities& caps, ButtonKind kind) noexcept
{
    if (kind == ButtonKind::Invalid) {
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

[[nodiscard]] bool*
HasThumbStick(GamepadCapabilities& caps, ThumbStickKind kind) noexcept
{
    if (kind == ThumbStickKind::Invalid) {
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

[[nodiscard]] std::tuple<GamepadMappings, std::string>
GetMappings(const GamepadUUID& uuid) noexcept
{
    return GetMappings(uuid.ToString());
}

[[nodiscard]] std::tuple<GamepadMappings, std::string>
GetMappings(const std::string& uuidString) noexcept
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
        ButtonKind::Invalid,
        ButtonKind::Invalid,
        ButtonKind::Invalid,
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
        ButtonKind::Invalid,
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
        ButtonKind::Invalid,
        ButtonKind::Invalid,
        ButtonKind::Invalid,
    }};
#endif

    mappings.axes = {{
        AxisMapper{ThumbStickKind::LeftStickX, ButtonKind::Invalid, ButtonKind::Invalid},
        AxisMapper{ThumbStickKind::LeftStickY, ButtonKind::Invalid, ButtonKind::Invalid},
        AxisMapper{ThumbStickKind::Invalid, ButtonKind::LeftTrigger, ButtonKind::Invalid},
        AxisMapper{ThumbStickKind::RightStickX, ButtonKind::Invalid, ButtonKind::Invalid},
        AxisMapper{ThumbStickKind::RightStickY, ButtonKind::Invalid, ButtonKind::Invalid},
        AxisMapper{ThumbStickKind::Invalid, ButtonKind::RightTrigger, ButtonKind::Invalid},
    }};

    std::string deviceName;
    for (auto m : s_ControllerMappings) {
        if (m == nullptr) {
            break;
        }
        std::string s = m;
        if (StringHelper::HasPrefix(s, uuidString)) {
            // found
            std::fill(std::begin(mappings.buttons), std::end(mappings.buttons), ButtonKind::Invalid);
            for (auto& axis : mappings.axes) {
                axis.thumbStick = ThumbStickKind::Invalid;
                axis.positiveTrigger = ButtonKind::Invalid;
                axis.negativeTrigger = ButtonKind::Invalid;
            }
            ParseMapping(m, mappings, deviceName);
            break;
        }
    }
    return std::make_tuple(mappings, deviceName);
}

} // namespace pomdog::detail
