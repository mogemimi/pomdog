// Copyright mogemimi. Distributed under the MIT license.

#pragma once

namespace pomdog::skeletal2d {
class Skeleton;
} // namespace pomdog::skeletal2d

namespace pomdog::spine {

class SkeletonData;

/// Creates a skeleton from the joints of a `.skeleton_skel2d` file.
[[nodiscard]] skeletal2d::Skeleton
CreateSkeleton(const SkeletonData& skeletonData);

} // namespace pomdog::spine
