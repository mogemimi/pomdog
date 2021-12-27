// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/skeletal2d/joint_pose.h"
#include "pomdog/math/matrix3x2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class Skeleton;

class SkeletonPose final {
public:
    std::vector<JointPose> JointPoses;

    static SkeletonPose CreateBindPose(const Skeleton& skeleton);
};

} // namespace pomdog::skeletal2d
