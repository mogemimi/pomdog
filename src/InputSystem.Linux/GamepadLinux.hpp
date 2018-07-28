// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "../InputSystem/GamepadMappings.hpp"
#include "../InputSystem/NativeGamepad.hpp"
#include "Pomdog/Input/Gamepad.hpp"
#include "Pomdog/Input/GamepadCapabilities.hpp"
#include "Pomdog/Input/GamepadState.hpp"
#include <array>
#include <cstdint>
#include <memory>

namespace Pomdog {
namespace Detail {
namespace InputSystem {
namespace Linux {

struct ThumbStickInfo final {
    int32_t Minimum = 0;
    int32_t Range = 0;
};

class GamepadDevice final {
public:
    int fd = -1;
    int deviceEventIndex = 0;
    GamepadCapabilities caps;
    GamepadState state;
    GamepadMappings mappings;
    std::array<int8_t, 16> keyMap;
    std::array<ThumbStickInfo, 6> thumbStickInfos;

public:
    bool Open(int deviceIndex);

    void Close();

    bool HasFileDescriptor() const;

    void PollEvents();
};

class GamepadLinux final : public NativeGamepad {
public:
    GamepadLinux();

    ~GamepadLinux();

    GamepadCapabilities GetCapabilities(PlayerIndex index) const override;

    GamepadState GetState(PlayerIndex index) const override;

    void EnumerateDevices();

    void PollEvents() override;

private:
    std::array<GamepadDevice, 4> gamepads;
};

} // namespace Linux
} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog
