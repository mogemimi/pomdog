// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/joint_index.h"
#include "pomdog/math/color.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/math/vector2.h"

namespace pomdog::skeletal2d {

class RigidSlot final {
public:
    Rect2D subrect;
    Vector2 translate;
    Vector2 scale;
    Radian<f32> rotation;
    pomdog::Color color;
    Vector2 origin;
    skeletal2d::JointIndex jointIndex;
    u32 hashID;
    i16 drawOrder;
    i16 texturePage;
    bool textureRotate;
};

} // namespace pomdog::skeletal2d
