//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPINE_SKELETONLOADER_20525A17_E478_4A36_B3F3_AF8095E19A1B_HPP
#define POMDOG_SPINE_SKELETONLOADER_20525A17_E478_4A36_B3F3_AF8095E19A1B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <vector>

namespace Pomdog {

class Skeleton;

namespace Details {
namespace Spine {

class BoneDesc;

Skeleton CreateSkeleton(std::vector<BoneDesc> const& bones);

}// namespace Spine
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SPINE_SKELETONLOADER_20525A17_E478_4A36_B3F3_AF8095E19A1B_HPP)
