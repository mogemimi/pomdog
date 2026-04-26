// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class Game;
} // namespace pomdog

namespace pomdog::gpu {
struct PresentationParameters;
} // namespace pomdog::gpu

namespace pomdog::detail::emscripten {

class GameHostEmscripten : public GameHost {
public:
    GameHostEmscripten() noexcept;

    ~GameHostEmscripten() override;

    virtual void
    run(Game& game) = 0;

    [[nodiscard]] static std::tuple<std::shared_ptr<GameHostEmscripten>, std::unique_ptr<Error>>
    create(
        const gpu::PresentationParameters& presentationParameters,
        const std::string& targetCanvas) noexcept;
};

} // namespace pomdog::detail::emscripten
