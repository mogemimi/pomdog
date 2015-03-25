// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_JOINT_0CE2C1D3_6959_40C5_B01B_7FA838BCB51B_HPP
#define POMDOG_JOINT_0CE2C1D3_6959_40C5_B01B_7FA838BCB51B_HPP

#if _MSC_VER > 1000
#pragma once
#endif

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

}// namespace Pomdog

#endif // !defined(POMDOG_JOINT_0CE2C1D3_6959_40C5_B01B_7FA838BCB51B_HPP)
