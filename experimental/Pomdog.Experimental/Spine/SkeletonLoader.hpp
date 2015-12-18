// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_SKELETONLOADER_20525A17_HPP
#define POMDOG_SKELETONLOADER_20525A17_HPP

#include <vector>

namespace Pomdog {

class Skeleton;

namespace Spine {

class BoneDesc;

Skeleton CreateSkeleton(std::vector<BoneDesc> const& bones);

} // namespace Spine
} // namespace Pomdog

#endif // POMDOG_SKELETONLOADER_20525A17_HPP
