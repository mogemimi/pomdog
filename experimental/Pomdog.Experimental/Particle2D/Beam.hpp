// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Color.hpp"
#include <vector>

namespace Pomdog {

class Beam {
public:
    std::vector<Vector2> Points;
    Pomdog::Color Color;
    float Thickness;
    float TimeToLive;
};

} // namespace Pomdog
