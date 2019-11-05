// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/JointIndex.hpp"
#include "Pomdog/Experimental/Skeletal2D/JointPose.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"

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
