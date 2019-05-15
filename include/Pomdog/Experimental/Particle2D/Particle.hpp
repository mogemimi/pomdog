// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include <cstdint>

namespace Pomdog {

class Particle final {
public:
    Vector3 Position{0.0f, 0.0f, 0.0f};
    Vector3 Velocity{1.0f, 1.0f, 1.0f};

    Pomdog::Color StartColor = Pomdog::Color::White;
    Pomdog::Color Color = Pomdog::Color::White;
    float ColorVariance = 0;

    Radian<float> Rotation = 0;
    float RotationVariance = 0;

    float StartSize = 1;
    float Size = 1;
    float SizeVariance = 0;

    float TimeToLive = 2;
};

} // namespace Pomdog
