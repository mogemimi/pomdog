// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_host.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/platform/win32/prerequisites_win32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class Game;
struct GameHostOptions;
} // namespace pomdog

namespace pomdog::gpu {
struct PresentationParameters;
enum class GraphicsBackend : u8;
} // namespace pomdog::gpu

namespace pomdog::detail {
class SystemEventQueue;
} // namespace pomdog::detail

namespace pomdog::detail::win32 {
class GameWindowWin32;
} // namespace pomdog::detail::win32

namespace pomdog::detail::win32 {

class GameHostWin32 : public GameHost {
public:
    GameHostWin32();

    virtual ~GameHostWin32();

    virtual void
    run(Game& game) = 0;

    [[nodiscard]] static std::tuple<std::shared_ptr<GameHostWin32>, std::unique_ptr<Error>>
    create(
        const std::shared_ptr<GameWindowWin32>& window,
        HINSTANCE hInstance,
        const std::shared_ptr<SystemEventQueue>& eventQueue,
        const gpu::PresentationParameters& presentationParameters,
        const GameHostOptions& options) noexcept;
};

} // namespace pomdog::detail::win32
