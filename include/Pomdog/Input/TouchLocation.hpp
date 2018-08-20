// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Input/TouchLocationState.hpp"
#include "Pomdog/Math/Point2D.hpp"
#include <cstdint>

namespace Pomdog {

struct TouchLocation final {
    Point2D Position;
    std::uint16_t Id;
    TouchLocationState State = TouchLocationState::Invalid;
};

} // namespace Pomdog
