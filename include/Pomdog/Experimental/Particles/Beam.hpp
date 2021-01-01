// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Vector2.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class Beam final {
public:
    std::vector<Vector2> Points;
    Pomdog::Color Color;
    float Thickness;
    float TimeToLive;
};

} // namespace Pomdog
