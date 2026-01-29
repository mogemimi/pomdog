// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/math/color.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector3.h"

namespace pomdog {

class Particle final {
public:
    Vector3 Position{0.0f, 0.0f, 0.0f};
    Vector3 Velocity{1.0f, 1.0f, 1.0f};

    pomdog::Color StartColor = pomdog::Color::createWhite();
    pomdog::Color Color = pomdog::Color::createWhite();
    f32 ColorVariance = 0.0f;

    Radian<f32> Rotation = 0.0f;
    f32 RotationVariance = 0.0f;

    f32 StartSize = 1.0f;
    f32 Size = 1.0f;
    f32 SizeVariance = 0.0f;

    f32 TimeToLive = 2.0f;
};

} // namespace pomdog
