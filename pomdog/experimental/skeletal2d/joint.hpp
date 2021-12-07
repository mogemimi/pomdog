// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/experimental/skeletal2d/joint_index.hpp"
#include "pomdog/experimental/skeletal2d/joint_pose.hpp"
#include "pomdog/math/matrix3x2.hpp"

namespace Pomdog::Skeletal2D {

struct Joint final {
    Matrix3x2 InverseBindPose;
    JointPose BindPose;
    JointIndex Index;
    JointIndex Parent;
    JointIndex FirstChild;
    JointIndex Sibling;
};

} // namespace Pomdog::Skeletal2D
