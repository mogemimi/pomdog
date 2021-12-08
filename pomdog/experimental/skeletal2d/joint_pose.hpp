// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/math/radian.hpp"
#include "pomdog/math/vector2.hpp"

namespace pomdog::skeletal2d {

struct JointPose final {
    Vector2 Translate;
    Radian<float> Rotation;
    float Scale;
};

} // namespace pomdog::skeletal2d
