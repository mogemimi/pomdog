//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_WEIGHTBLENDINGHELPER_EC5A3515_C7FB_4F93_A810_79F3F8F59CEE_HPP
#define POMDOG_WEIGHTBLENDINGHELPER_EC5A3515_C7FB_4F93_A810_79F3F8F59CEE_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <vector>

namespace Pomdog {

class JointPose;

namespace Details {
namespace Skeletal2D {

class WeightBlendingHelper {
public:
	static void Lerp(std::vector<JointPose> const& sourcePoses1,
		std::vector<JointPose> const& sourcePoses2,
		float weight, std::vector<JointPose> & output);
};

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_WEIGHTBLENDINGHELPER_EC5A3515_C7FB_4F93_A810_79F3F8F59CEE_HPP)
