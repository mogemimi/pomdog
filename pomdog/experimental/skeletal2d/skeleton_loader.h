// Copyright mogemimi. Distributed under the MIT license.

#pragma once

namespace pomdog::skeletal2d {

class Skeleton;
class SkeletonData;

/// Creates a skeleton from the joints of a `.skeleton_skel2d` file.
[[nodiscard]] Skeleton
createSkeleton(const SkeletonData& skeletonData);

} // namespace pomdog::skeletal2d
