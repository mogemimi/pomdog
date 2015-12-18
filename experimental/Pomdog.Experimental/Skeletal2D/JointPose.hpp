// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Radian.hpp"

namespace Pomdog {

class JointPose {
public:
    Vector2 Translate;
    Radian<float> Rotation;
    float Scale;
};

} // namespace Pomdog
