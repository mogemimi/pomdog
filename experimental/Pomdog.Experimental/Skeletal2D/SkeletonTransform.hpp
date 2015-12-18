// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "SkeletonPose.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include <vector>

namespace Pomdog {

class SkeletonTransform {
public:
    SkeletonPose Pose;
    std::vector<Matrix3x2> GlobalPose;
};

} // namespace Pomdog
