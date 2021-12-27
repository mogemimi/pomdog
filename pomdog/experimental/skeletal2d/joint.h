// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/experimental/skeletal2d/joint_index.h"
#include "pomdog/experimental/skeletal2d/joint_pose.h"
#include "pomdog/math/matrix3x2.h"

namespace pomdog::skeletal2d {

struct Joint final {
    Matrix3x2 InverseBindPose;
    JointPose BindPose;
    JointIndex Index;
    JointIndex Parent;
    JointIndex FirstChild;
    JointIndex Sibling;
};

} // namespace pomdog::skeletal2d
