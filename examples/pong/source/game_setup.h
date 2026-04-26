#pragma once

#include "pomdog/application/game_setup.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pong {

[[nodiscard]] std::unique_ptr<pomdog::GameSetup>
createGameSetup();

} // namespace pong
