// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/backends/gamepad_mapping_entry.h"
#include "pomdog/input/gamepad_capabilities.h"
#include "pomdog/input/gamepad_service.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstdint>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class GameControllerDB;
class GamepadImpl;
} // namespace pomdog

namespace pomdog::detail::linux {

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
    [[nodiscard]] bool
    open(int deviceIndex, const GameControllerDB& gameControllerDB);

    void close();

    [[nodiscard]] bool
    hasFileDescriptor() const;

    [[nodiscard]] bool
    pollEvents();
};

class GamepadServiceLinux final : public GamepadService {
private:
    std::shared_ptr<const GameControllerDB> gameControllerDB_;
    std::array<GamepadDevice, 4> gamepads_;

public:
    GamepadServiceLinux() noexcept;

    ~GamepadServiceLinux() override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(std::shared_ptr<const GameControllerDB> gameControllerDB) noexcept;

    [[nodiscard]] std::shared_ptr<Gamepad>
    getGamepad(PlayerIndex playerIndex) noexcept override;

    void enumerateDevices();

    void pollEvents();
};

} // namespace pomdog::detail::linux
