// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Gamepad;
enum class PlayerIndex : i8;
} // namespace pomdog

namespace pomdog {

/// Manages gamepad devices for the current platform.
///
/// Handles platform-specific device enumeration, event polling,
/// and connect/disconnect notifications. Use getGamepad() to
/// retrieve a Gamepad instance by player index.
class POMDOG_EXPORT GamepadService {
public:
    GamepadService() noexcept;

    GamepadService(const GamepadService&) = delete;
    GamepadService& operator=(const GamepadService&) = delete;

    virtual ~GamepadService();

    /// Returns the gamepad for the given player index.
    [[nodiscard]] virtual std::shared_ptr<Gamepad>
    getGamepad(PlayerIndex playerIndex) noexcept = 0;
};

} // namespace pomdog
