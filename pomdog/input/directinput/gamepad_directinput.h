// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/backends/gamepad_mapping_entry.h"
#include "pomdog/input/directinput/prerequisites_directinput.h"
#include "pomdog/input/gamepad_capabilities.h"
#include "pomdog/input/gamepad_service.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
#include <array>
#include <cstdint>
#include <memory>
#include <mutex>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class GameControllerDB;
} // namespace pomdog

namespace pomdog::detail {
class GamepadImpl;
} // namespace pomdog::detail

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
    std::shared_ptr<GamepadImpl> impl;
    GamepadStateDirectInput deviceState;

    Microsoft::WRL::ComPtr<IDirectInputDevice8> inputDevice;
    ::GUID deviceGuid;

    GamepadMappingEntry mappings;
    std::array<ThumbStickInfo, 6> thumbStickInfos;
    PlayerIndex playerIndex;
    bool isXInputDevice;

    [[nodiscard]] bool
    open(IDirectInput8* directInput, HWND windowHandle, const ::GUID& guidInstance,
        const GameControllerDB& gameControllerDB);

    void close();

    void pollEvents();
};

class GamepadServiceDirectInput final : public GamepadService {
private:
    std::shared_ptr<const GameControllerDB> gameControllerDB_;
    std::array<GamepadDevice, 4> gamepads_;
    HWND windowHandle_ = nullptr;
    Microsoft::WRL::ComPtr<IDirectInput8> directInput_;
    std::mutex mutex_;

public:
    ~GamepadServiceDirectInput();

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        HINSTANCE hInstance,
        HWND windowHandle,
        std::shared_ptr<const GameControllerDB> gameControllerDB) noexcept;

    [[nodiscard]] std::shared_ptr<Gamepad>
    getGamepad(PlayerIndex playerIndex) noexcept override;

    void pollEvents();

    void enumerateDevices();

    BOOL onDeviceAttached(LPCDIDEVICEINSTANCE deviceInstance);
};

} // namespace pomdog::detail::directinput
