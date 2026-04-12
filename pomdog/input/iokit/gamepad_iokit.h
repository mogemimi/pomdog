// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/backends/gamepad_mapping_entry.h"
#include "pomdog/input/gamepad_capabilities.h"
#include "pomdog/input/gamepad_service.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <IOKit/hid/IOHIDManager.h>
#include <array>
#include <cstdint>
#include <memory>
#include <mutex>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class GameControllerDB;
class GamepadImpl;
} // namespace pomdog

namespace pomdog::detail::IOKit {

struct ThumbStickInfo final {
    std::int32_t minimum = 0;
    std::int32_t range = 0;
    std::int32_t invertDirection = 1;
};

class GamepadDevice final {
public:
    GamepadCapabilities caps;
    std::shared_ptr<GamepadImpl> impl;
    std::shared_ptr<GamepadImpl> backImpl;
    std::mutex mutex;
    IOHIDDeviceRef device = nullptr;
    GamepadMappingEntry mappings;
    std::array<ThumbStickInfo, 6> thumbStickInfos;
    PlayerIndex playerIndex;

public:
    void close();

    void flushState();

    void onDeviceInput(IOReturn result, void* sender, IOHIDValueRef value);
};

class GamepadServiceIOKit final : public GamepadService {
private:
    std::shared_ptr<const GameControllerDB> gameControllerDB_;
    std::array<GamepadDevice, 4> gamepads_;
    IOHIDManagerRef hidManager_ = nullptr;

public:
    GamepadServiceIOKit();

    ~GamepadServiceIOKit() override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(std::shared_ptr<const GameControllerDB> gameControllerDB);

    [[nodiscard]] std::shared_ptr<Gamepad>
    getGamepad(PlayerIndex playerIndex) noexcept override;

    void pollEvents();

private:
    void onDeviceAttached(IOReturn result, void* sender, IOHIDDeviceRef device);
    void onDeviceDetached(IOReturn result, void* sender, IOHIDDeviceRef device);
};

} // namespace pomdog::detail::IOKit
