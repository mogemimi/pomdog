// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/backends/gamepad_mappings.h"
#include "pomdog/input/directinput/prerequisites_directinput.h"
#include "pomdog/input/gamepad.h"
#include "pomdog/input/gamepad_capabilities.h"
#include "pomdog/input/gamepad_state.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
#include <array>
#include <cstdint>
#include <memory>
#include <mutex>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::directinput {

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

    [[nodiscard]] bool
    open(IDirectInput8* directInput, HWND windowHandle, const ::GUID& guidInstance);

    void close();

    void pollEvents();
};

class GamepadDirectInput final : public Gamepad {
private:
    std::array<GamepadDevice, 4> gamepads_;
    HWND windowHandle_ = nullptr;
    Microsoft::WRL::ComPtr<IDirectInput8> directInput_;
    std::mutex mutex_;

public:
    ~GamepadDirectInput();

    [[nodiscard]] std::unique_ptr<Error>
    initialize(HINSTANCE hInstance, HWND windowHandle) noexcept;

    [[nodiscard]] GamepadCapabilities
    getCapabilities(PlayerIndex index) const override;

    [[nodiscard]] GamepadState
    getState(PlayerIndex index) const override;

    void pollEvents();

    void enumerateDevices();

    BOOL onDeviceAttached(LPCDIDEVICEINSTANCE deviceInstance);
};

} // namespace pomdog::detail::directinput
