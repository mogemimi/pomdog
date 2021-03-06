// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Math/Vector2.hpp"

namespace Pomdog::Skeletal2D {

struct JointPose final {
    Vector2 Translate;
    Radian<float> Rotation;
    float Scale;
};

} // namespace Pomdog::Skeletal2D
