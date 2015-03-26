// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SPINE_SKELETONLOADER_20525A17_E478_4A36_B3F3_AF8095E19A1B_HPP
#define POMDOG_SPINE_SKELETONLOADER_20525A17_E478_4A36_B3F3_AF8095E19A1B_HPP

#include <vector>

namespace Pomdog {

class Skeleton;

namespace Spine {

class BoneDesc;

Skeleton CreateSkeleton(std::vector<BoneDesc> const& bones);

}// namespace Spine
}// namespace Pomdog

#endif // !defined(POMDOG_SPINE_SKELETONLOADER_20525A17_E478_4A36_B3F3_AF8095E19A1B_HPP)
