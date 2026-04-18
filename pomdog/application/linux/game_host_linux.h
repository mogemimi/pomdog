// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Game;
class Error;
} // namespace pomdog

namespace pomdog::gpu {
struct PresentationParameters;
} // namespace pomdog::gpu

namespace pomdog::detail::linux {

class GameHostLinux : public GameHost {
public:
    GameHostLinux() noexcept;

    ~GameHostLinux() override;

    virtual void
    run(Game& game) = 0;

    [[nodiscard]] static std::tuple<std::shared_ptr<GameHostLinux>, std::unique_ptr<Error>>
    create(const gpu::PresentationParameters& presentationParameters) noexcept;
};

} // namespace pomdog::detail::linux
