// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Radian.hpp"

namespace Pomdog {

struct JointPose {
    Vector2 Translate;
    Radian<float> Rotation;
    float Scale;
};

} // namespace Pomdog
