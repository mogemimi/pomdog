// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_JOINT_0CE2C1D3_HPP
#define POMDOG_JOINT_0CE2C1D3_HPP

#include "JointIndex.hpp"
#include "JointPose.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"

namespace Pomdog {

class Joint {
public:
    Matrix3x2 InverseBindPose;
    JointPose BindPose;
    JointIndex Index;
    JointIndex Parent;
    JointIndex FirstChild;
    JointIndex Sibling;
};

} // namespace Pomdog

#endif // POMDOG_JOINT_0CE2C1D3_HPP
