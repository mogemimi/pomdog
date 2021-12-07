// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/math/color.hpp"
#include "pomdog/math/radian.hpp"
#include "pomdog/math/vector3.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class Particle final {
public:
    Vector3 Position{0.0f, 0.0f, 0.0f};
    Vector3 Velocity{1.0f, 1.0f, 1.0f};

    Pomdog::Color StartColor = Pomdog::Color::White;
    Pomdog::Color Color = Pomdog::Color::White;
    float ColorVariance = 0.0f;

    Radian<float> Rotation = 0.0f;
    float RotationVariance = 0.0f;

    float StartSize = 1.0f;
    float Size = 1.0f;
    float SizeVariance = 0.0f;

    float TimeToLive = 2.0f;
};

} // namespace Pomdog
