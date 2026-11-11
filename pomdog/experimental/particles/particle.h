// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/math/color.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector3.h"

namespace pomdog {

class Particle final {
public:
    Vector3 position{0.0f, 0.0f, 0.0f};
    Vector3 velocity{1.0f, 1.0f, 1.0f};

    Color startColor = Color::createWhite();
    Color color = Color::createWhite();
    f32 colorVariance = 0.0f;

    Radian<f32> rotation = 0.0f;
    f32 rotationVariance = 0.0f;

    f32 startSize = 1.0f;
    f32 size = 1.0f;
    f32 sizeVariance = 0.0f;

    f32 timeToLive = 2.0f;
};

} // namespace pomdog
