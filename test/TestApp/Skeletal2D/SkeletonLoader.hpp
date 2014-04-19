//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SKELETONLOADER_20525A17_E478_4A36_B3F3_AF8095E19A1B_HPP
#define POMDOG_SKELETONLOADER_20525A17_E478_4A36_B3F3_AF8095E19A1B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <vector>

namespace Pomdog {

class Skeleton;

namespace Details {
namespace Skeletal2D {

class BoneDesc;

Skeleton CreateSkeleton(std::vector<BoneDesc> const& bones);

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SKELETONLOADER_20525A17_E478_4A36_B3F3_AF8095E19A1B_HPP)
