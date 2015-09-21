// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PARTICLE_3943827E_HPP
#define POMDOG_PARTICLE_3943827E_HPP

#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Radian.hpp"
#include <cstdint>

namespace Pomdog {

class Particle {
public:
    Vector2 Position {0, 0};
    Vector2 Velocity {1, 1};

    Color StartColor = Color::White;
    Color Color = Color::White;
    float ColorVariance = 0;

    Radian<float> Rotation = 0;
    float RotationVariance = 0;

    float StartSize = 1;
    float Size = 1;
    float SizeVariance = 0;

    float TimeToLive = 2;
};

} // namespace Pomdog

#endif // POMDOG_PARTICLE_3943827E_HPP
