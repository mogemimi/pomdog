// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/skeletal2d/joint_index.hpp"
#include "pomdog/math/matrix3x2.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {
class Skeleton;
class SkeletonPose;
struct Joint;
} // namespace pomdog::skeletal2d

namespace pomdog::skeletal2d::SkeletonHelper {

void Traverse(
    const Skeleton& skeleton,
    const JointIndex& jointIndex,
    const std::function<void(Joint const&)>& traverser);

void Traverse(
    const Skeleton& skeleton,
    const std::function<void(Joint const&)>& traverser);

void ToGlobalPose(
    const Skeleton& skeleton,
    const SkeletonPose& skeletonPose,
    std::vector<Matrix3x2>& globalPose);

[[nodiscard]] std::vector<Matrix3x2>
ToGlobalPose(
    const Skeleton& skeleton,
    const SkeletonPose& skeletonPose);

} // namespace pomdog::skeletal2d::SkeletonHelper
