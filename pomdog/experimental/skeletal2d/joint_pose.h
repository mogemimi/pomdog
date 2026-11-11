// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector2.h"

namespace pomdog::skeletal2d {

struct JointPose final {
    Vector2 translate;
    Radian<f32> rotation;
    f32 scale;
};

} // namespace pomdog::skeletal2d
