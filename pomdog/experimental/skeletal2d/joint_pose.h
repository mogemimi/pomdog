// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/math/turn.h"
#include "pomdog/math/vector2.h"

namespace pomdog::skeletal2d {

struct JointPose final {
    Vector2 translate;
    Turn32 rotation;
    f32 scale;
};

} // namespace pomdog::skeletal2d
