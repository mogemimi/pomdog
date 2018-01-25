// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "JointIndex.hpp"
#include "JointPose.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"

namespace Pomdog {

struct Joint {
    Matrix3x2 InverseBindPose;
    JointPose BindPose;
    JointIndex Index;
    JointIndex Parent;
    JointIndex FirstChild;
    JointIndex Sibling;
};

} // namespace Pomdog
