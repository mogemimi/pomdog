// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_WEIGHTBLENDINGHELPER_EC5A3515_C7FB_4F93_A810_79F3F8F59CEE_HPP
#define POMDOG_WEIGHTBLENDINGHELPER_EC5A3515_C7FB_4F93_A810_79F3F8F59CEE_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <vector>

namespace Pomdog {

class JointPose;

namespace Detail {
namespace Skeletal2D {

class WeightBlendingHelper {
public:
	static void Lerp(std::vector<JointPose> const& sourcePoses1,
		std::vector<JointPose> const& sourcePoses2,
		float weight, std::vector<JointPose> & output);
};

}// namespace Skeletal2D
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_WEIGHTBLENDINGHELPER_EC5A3515_C7FB_4F93_A810_79F3F8F59CEE_HPP)
