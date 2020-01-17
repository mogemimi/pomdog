// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/JointPose.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include <vector>

namespace Pomdog::Skeletal2D {

class Skeleton;

class SkeletonPose final {
public:
    std::vector<JointPose> JointPoses;

    static SkeletonPose CreateBindPose(const Skeleton& skeleton);
};

} // namespace Pomdog::Skeletal2D
