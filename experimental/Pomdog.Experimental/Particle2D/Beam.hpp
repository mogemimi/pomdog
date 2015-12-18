// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_BEAM_463AA6E5_HPP
#define POMDOG_BEAM_463AA6E5_HPP

#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Color.hpp"
#include <vector>

namespace Pomdog {

class Beam {
public:
    std::vector<Vector2> Points;
    Color Color;
    float Thickness;
    float TimeToLive;
};

} // namespace Pomdog

#endif // POMDOG_BEAM_463AA6E5_HPP
