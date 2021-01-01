// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirectInput.hpp"
#include "../Input.Backends/GamepadMappings.hpp"
#include "../Input.Backends/NativeGamepad.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Input/Gamepad.hpp"
#include "Pomdog/Input/GamepadCapabilities.hpp"
#include "Pomdog/Input/GamepadState.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
#include <array>
#include <cstdint>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
class Error;
} // namespace Pomdog

namespace Pomdog::Detail::DirectInput {

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
    ~GamepadDirectInput();

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(HINSTANCE hInstance, HWND windowHandle) noexcept;

    GamepadCapabilities GetCapabilities(PlayerIndex index) const override;

    GamepadState GetState(PlayerIndex index) const override;

    void PollEvents() override;

    void EnumerateDevices();

    BOOL OnDeviceAttached(LPCDIDEVICEINSTANCE deviceInstance);

private:
    std::array<GamepadDevice, 4> gamepads;
    HWND windowHandle = nullptr;
    Microsoft::WRL::ComPtr<IDirectInput8> directInput;
};

} // namespace Pomdog::Detail::DirectInput
