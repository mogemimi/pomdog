// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <vector>

namespace Pomdog {

class Beam final {
public:
    std::vector<Vector2> Points;
    Pomdog::Color Color;
    float Thickness;
    float TimeToLive;
};

} // namespace Pomdog
