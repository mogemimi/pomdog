// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "JointIndex.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include <vector>
#include <functional>

namespace Pomdog {

class Skeleton;
class SkeletonPose;
struct Joint;

namespace SkeletonHelper {
    void Traverse(
        Skeleton const& skeleton,
        JointIndex const& jointIndex,
        std::function<void(Joint const&)> const& traverser);

    void Traverse(
        Skeleton const& skeleton,
        std::function<void(Joint const&)> const& traverser);

    void ToGlobalPose(
        Skeleton const& skeleton,
        SkeletonPose const& skeletonPose,
        std::vector<Matrix3x2> & globalPose);

    std::vector<Matrix3x2> ToGlobalPose(
        Skeleton const& skeleton, SkeletonPose const& skeletonPose);
}

} // namespace Pomdog
