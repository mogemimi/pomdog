// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/skeletal2d/joint_index.h"
#include "pomdog/math/color.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class RigidSlot final {
public:
    Rectangle Subrect;
    Vector2 Translate;
    Vector2 Scale;
    Radian<float> Rotation;
    pomdog::Color Color;
    Vector2 Origin;
    skeletal2d::JointIndex JointIndex;
    std::uint32_t HashID;
    std::int16_t DrawOrder;
    std::int16_t TexturePage;
    bool TextureRotate;
};

} // namespace pomdog::skeletal2d
