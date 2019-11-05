// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/JointIndex.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include <functional>
#include <vector>

namespace Pomdog::Skeletal2D {
class Skeleton;
class SkeletonPose;
struct Joint;
} // namespace Pomdog::Skeletal2D

namespace Pomdog::Skeletal2D::SkeletonHelper {

void
Traverse(
    const Skeleton& skeleton,
    const JointIndex& jointIndex,
    const std::function<void(Joint const&)>& traverser);

void
Traverse(
    const Skeleton& skeleton,
    const std::function<void(Joint const&)>& traverser);

void
ToGlobalPose(
    const Skeleton& skeleton,
    const SkeletonPose& skeletonPose,
    std::vector<Matrix3x2>& globalPose);

[[nodiscard]] std::vector<Matrix3x2>
ToGlobalPose(
    const Skeleton& skeleton,
    const SkeletonPose& skeletonPose);

} // namespace Pomdog::Skeletal2D::SkeletonHelper
