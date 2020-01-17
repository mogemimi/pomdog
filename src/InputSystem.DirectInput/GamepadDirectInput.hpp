// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirectInput.hpp"
#include "../InputSystem/GamepadMappings.hpp"
#include "../InputSystem/NativeGamepad.hpp"
#include "Pomdog/Input/Gamepad.hpp"
#include "Pomdog/Input/GamepadCapabilities.hpp"
#include "Pomdog/Input/GamepadState.hpp"
#include <wrl/client.h>
#include <array>
#include <cstdint>
#include <memory>

namespace Pomdog::Detail::InputSystem::DirectInput {

enum class GamepadStateDirectInput : std::uint8_t {
    NotInitialized,
    NotAcquired,
    Acquired,
};

struct ThumbStickInfo final {
    std::int32_t InvertDirection = 1;
};

struct GamepadDevice final {
    GamepadCapabilities caps;
    GamepadState state;
    GamepadStateDirectInput deviceState;

    Microsoft::WRL::ComPtr<IDirectInputDevice8> inputDevice;
    ::GUID deviceGuid;

    GamepadMappings mappings;
    std::array<ThumbStickInfo, 6> thumbStickInfos;
    PlayerIndex playerIndex;
    bool isXInputDevice;

    bool Open(IDirectInput8* directInput, HWND windowHandle, const ::GUID& guidInstance);

    void Close();

    void PollEvents();
};

class GamepadDirectInput final : public NativeGamepad {
public:
    GamepadDirectInput(HINSTANCE hInstance, HWND windowHandle);

    ~GamepadDirectInput();

    GamepadCapabilities GetCapabilities(PlayerIndex index) const override;

    GamepadState GetState(PlayerIndex index) const override;

    void PollEvents() override;

    void EnumerateDevices();

    BOOL OnDeviceAttached(LPCDIDEVICEINSTANCE deviceInstance);

private:
    std::array<GamepadDevice, 4> gamepads;
    HWND windowHandle;
    Microsoft::WRL::ComPtr<IDirectInput8> directInput;
};

} // namespace Pomdog::Detail::InputSystem::DirectInput
