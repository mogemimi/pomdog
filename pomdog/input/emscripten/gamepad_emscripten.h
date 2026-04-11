// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/gamepad_service.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
class GamepadImpl;
} // namespace pomdog::detail

namespace pomdog::detail::emscripten {

class GamepadServiceEmscripten final : public GamepadService {
private:
    static constexpr int maxGamepads = 4;
    std::array<std::shared_ptr<GamepadImpl>, maxGamepads> gamepads_;

public:
    GamepadServiceEmscripten() noexcept;

    ~GamepadServiceEmscripten() noexcept override;

    [[nodiscard]] std::shared_ptr<Gamepad>
    getGamepad(PlayerIndex playerIndex) noexcept override;

    void pollEvents() noexcept;
};

} // namespace pomdog::detail::emscripten
