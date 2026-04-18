// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_window.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::emscripten {

class GameWindowEmscripten : public GameWindow {
public:
    GameWindowEmscripten() noexcept;

    ~GameWindowEmscripten() noexcept override;

    [[nodiscard]] virtual const std::string&
    getTargetCanvas() const noexcept = 0;

    [[nodiscard]] static std::tuple<std::shared_ptr<GameWindowEmscripten>, std::unique_ptr<Error>>
    create(const std::string& targetCanvas, int width, int height) noexcept;
};

} // namespace pomdog::detail::emscripten
