// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/input/backends/gamepad_mapping_entry.h"
#include "pomdog/input/gamepad_capabilities.h"
#include "pomdog/input/gamepad_service.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstdint>
#include <memory>
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

namespace pomdog::detail::linux {

/// GamepadOpenResult tells whether GamepadDevice::open() opened the device.
enum class GamepadOpenResult : u8 {
    /// The device is open as a gamepad.
    Opened,

    /// The device is not a gamepad, such as a keyboard, or was removed while
    /// it was being opened. It stays closed.
    NotGamepad,
};

struct ThumbStickInfo final {
    std::int32_t minimum = 0;
    std::int32_t range = 0;
    std::int32_t invertDirection = 1;
};

class GamepadDevice final {
public:
    int fd = -1;
    int deviceEventIndex = -1;
    GamepadCapabilities caps;
    std::shared_ptr<GamepadImpl> impl;
    GamepadMappingEntry mappings;
    std::array<int8_t, 32> keyMap;
    std::array<ThumbStickInfo, 6> thumbStickInfos;
    PlayerIndex playerIndex;

public:
    /// Opens /dev/input/event{deviceIndex} as a gamepad.
    ///
    /// Returns GamepadOpenResult::NotGamepad for a device that is not a
    /// gamepad, and an error when the device cannot be opened or queried. A
    /// device that cannot be opened for lack of permission is a gamepad only
    /// if sysfs describes it as one. In every case except Opened the device
    /// stays closed.
    [[nodiscard]] std::tuple<GamepadOpenResult, std::unique_ptr<Error>>
    open(int deviceIndex, const GameControllerDB& gameControllerDB);

    void close();

    [[nodiscard]] bool
    hasFileDescriptor() const;

    [[nodiscard]] bool
    pollEvents();
};

class GamepadServiceLinux final : public GamepadService {
private:
    struct SkippedDevice final {
        int deviceIndex = -1;

        // NOTE: The inode of the device node. The kernel creates a new node
        // when a device is plugged back in, so a different inode at the same
        // index means a different device.
        u64 inode = 0;

        // NOTE: chmod, chown, and ACL updates change ctime without replacing
        // the node. Retry when udev or logind grants access to the device.
        i64 changeTimeSeconds = 0;
        i64 changeTimeNanoseconds = 0;

        bool operator==(const SkippedDevice&) const noexcept = default;
    };

    std::shared_ptr<const GameControllerDB> gameControllerDB_;
    std::array<GamepadDevice, 4> gamepads_;

    // NOTE: Devices that are not gamepads or failed to open. They are skipped
    // while the node and its metadata stay the same. A reconnect or permission
    // change lets the next enumeration try again.
    std::vector<SkippedDevice> skippedDevices_;

    std::shared_ptr<DiagnosticQueue> diagnostics_;

public:
    GamepadServiceLinux() noexcept;

    ~GamepadServiceLinux() override;

    /// Initializes the service. `diagnostics` receives failures that the
    /// service handles on its own and must not be null.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        std::shared_ptr<const GameControllerDB> gameControllerDB,
        std::shared_ptr<DiagnosticQueue> diagnostics) noexcept;

    [[nodiscard]] std::shared_ptr<Gamepad>
    getGamepad(PlayerIndex playerIndex) noexcept override;

    /// Opens newly attached gamepads.
    ///
    /// Reports a diagnostic for a device that fails to open and skips it
    /// until it is plugged back in or its permissions change. Devices that
    /// are not gamepads are skipped the same way without a diagnostic.
    void enumerateDevices();

    void pollEvents();
};

} // namespace pomdog::detail::linux
