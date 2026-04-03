// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/backends/gamepad_mapping_entry.h"
#include "pomdog/input/gamepad.h"
#include "pomdog/input/gamepad_capabilities.h"
#include "pomdog/input/gamepad_state.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstdint>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class GameControllerDB;
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
    GamepadState state;
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

class GamepadLinux final : public Gamepad {
private:
    std::shared_ptr<const GameControllerDB> gameControllerDB_;
    std::array<GamepadDevice, 4> gamepads_;

public:
    GamepadLinux() noexcept;

    ~GamepadLinux() override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(std::shared_ptr<const GameControllerDB> gameControllerDB) noexcept;

    [[nodiscard]] GamepadCapabilities
    getCapabilities(PlayerIndex index) const override;

    [[nodiscard]] GamepadState
    getState(PlayerIndex index) const override;

    void enumerateDevices();

    void pollEvents();
};

} // namespace pomdog::detail::linux
