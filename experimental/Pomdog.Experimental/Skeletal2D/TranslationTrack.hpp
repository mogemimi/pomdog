//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_TRANSLATIONTRACK_6A0FA1BF_E029_40A4_BBB4_80B20013D984_HPP
#define POMDOG_TRANSLATIONTRACK_6A0FA1BF_E029_40A4_BBB4_80B20013D984_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "AnimationTrack.hpp"
#include "AnimationTimeInterval.hpp"
#include "CompressedFloat.hpp"
#include "JointIndex.hpp"
#include <vector>

namespace Pomdog {
namespace Details {
namespace Skeletal2D {

class TranslationKeyframe {
public:
	AnimationTimeInterval Time;
	CompressedFloat<std::int16_t, 7> TranslateX;
	CompressedFloat<std::int16_t, 7> TranslateY;

	static_assert(decltype(TranslateX)::Max() > 1024.0f, "");
	static_assert(decltype(TranslateX)::Min() < -1024.0f, "");
	static_assert(decltype(TranslateY)::Max() > 1024.0f, "");
	static_assert(decltype(TranslateY)::Min() < -1024.0f, "");
};

class TranslationTrack final: public AnimationTrack {
public:
	TranslationTrack(std::vector<TranslationKeyframe> && keys, JointIndex && jointIndex);

	void Apply(AnimationTimeInterval const& time, Skeleton const& skeleton,
		SkeletonPose & skeletonPose) override;
	
	AnimationTimeInterval Length() const override;
	
private:
	std::vector<TranslationKeyframe> keys;
	JointIndex jointIndex;
};

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_TRANSLATIONTRACK_6A0FA1BF_E029_40A4_BBB4_80B20013D984_HPP)
