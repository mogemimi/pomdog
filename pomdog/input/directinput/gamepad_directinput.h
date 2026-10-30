// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/input/backends/gamepad_mapping_entry.h"
#include "pomdog/input/directinput/prerequisites_directinput.h"
#include "pomdog/input/gamepad_capabilities.h"
#include "pomdog/input/gamepad_service.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
#include <array>
#include <memory>
#include <mutex>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class GameControllerDB;
} // namespace pomdog

namespace pomdog::detail {
class DiagnosticQueue;
class GamepadImpl;
} // namespace pomdog::detail

namespace pomdog::detail::directinput {

enum class GamepadStateDirectInput : u8 {
    NotInitialized,
    NotAcquired,
    Acquired,
};

/// GamepadOpenResult tells whether GamepadDevice::open() opened the device.
enum class GamepadOpenResult : u8 {
    /// The device is open as a gamepad.
    Opened,

    /// The device is not a gamepad, such as a device without buttons, and
    /// stays closed.
    NotGamepad,
};

struct ThumbStickInfo final {
    i32 InvertDirection = 1;
};

struct GamepadDevice final {
    GamepadCapabilities caps;
    std::shared_ptr<GamepadImpl> impl;
    GamepadStateDirectInput deviceState;

    Microsoft::WRL::ComPtr<IDirectInputDevice8> inputDevice;
    ::GUID deviceGuid;

    GamepadMappingEntry mappings;
    std::array<ThumbStickInfo, 6> thumbStickInfos;
    PlayerIndex playerIndex;
    bool isXInputDevice;

    /// Opens the device as a gamepad.
    ///
    /// Returns GamepadOpenResult::NotGamepad for a device that is not a
    /// gamepad, and an error when a DirectInput call fails. In both cases the
    /// device stays closed.
    [[nodiscard]] std::tuple<GamepadOpenResult, std::unique_ptr<Error>>
    open(
        IDirectInput8* directInput,
        HWND windowHandle,
        const ::GUID& guidInstance,
        const GameControllerDB& gameControllerDB);

    void close();

    void pollEvents();
};

class GamepadServiceDirectInput final : public GamepadService {
private:
    std::shared_ptr<const GameControllerDB> gameControllerDB_;
    std::array<GamepadDevice, 4> gamepads_;

    // NOTE: Devices that are not gamepads or failed to open. They are skipped
    // while they stay attached and tried again after they are plugged back in.
    std::vector<::GUID> skippedDevices_;

    // NOTE: Devices reported by the current enumeration pass.
    std::vector<::GUID> attachedDevices_;

    std::shared_ptr<DiagnosticQueue> diagnostics_;
    HWND windowHandle_ = nullptr;
    Microsoft::WRL::ComPtr<IDirectInput8> directInput_;
    std::mutex mutex_;
    bool enumerationStopped_ = false;

public:
    ~GamepadServiceDirectInput() override;

    /// Initializes the service. `diagnostics` receives failures that the
    /// service handles on its own and must not be null.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        HINSTANCE hInstance,
        HWND windowHandle,
        std::shared_ptr<const GameControllerDB> gameControllerDB,
        std::shared_ptr<DiagnosticQueue> diagnostics) noexcept;

    [[nodiscard]] std::shared_ptr<Gamepad>
    getGamepad(PlayerIndex playerIndex) noexcept override;

    void pollEvents();

    /// Opens newly attached gamepads. Called on the gamepad thread.
    ///
    /// Reports a diagnostic for a device that fails to open and skips it
    /// until it is unplugged. If the enumeration itself fails, reports a
    /// diagnostic and stops detecting new devices; gamepads that are already
    /// open keep working.
    void enumerateDevices();

    BOOL onDeviceAttached(LPCDIDEVICEINSTANCE deviceInstance);
};

} // namespace pomdog::detail::directinput
