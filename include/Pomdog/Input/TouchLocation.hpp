// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Input/TouchLocationState.hpp"
#include "Pomdog/Math/Point2D.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

struct TouchLocation final {
    Point2D Position;
    std::uint16_t Id;
    TouchLocationState State = TouchLocationState::Invalid;
};

} // namespace Pomdog
