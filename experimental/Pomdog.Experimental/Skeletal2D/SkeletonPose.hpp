// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_SKELETONPOSE_A5B94631_HPP
#define POMDOG_SKELETONPOSE_A5B94631_HPP

#include "JointPose.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include <vector>

namespace Pomdog {

class Skeleton;

class SkeletonPose {
public:
    std::vector<JointPose> JointPoses;

    static SkeletonPose CreateBindPose(Skeleton const& skeleton);
};

} // namespace Pomdog

#endif // POMDOG_SKELETONPOSE_A5B94631_HPP
