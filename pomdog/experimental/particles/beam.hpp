// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/math/color.hpp"
#include "pomdog/math/vector2.hpp"

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
