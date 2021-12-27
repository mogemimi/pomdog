// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/math/radian.h"
#include "pomdog/math/vector2.h"

namespace pomdog::skeletal2d {

struct JointPose final {
    Vector2 Translate;
    Radian<float> Rotation;
    float Scale;
};

} // namespace pomdog::skeletal2d
