// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ANIMATIONCLIPNODE_6B87052F_753A_4B60_B736_400EBF81491F_HPP
#define POMDOG_ANIMATIONCLIPNODE_6B87052F_753A_4B60_B736_400EBF81491F_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog.Experimental/Skeletal2D/AnimationNode.hpp"
#include <memory>

namespace Pomdog {

class AnimationClip;

namespace Detail {
namespace Skeletal2D {

class AnimationClipNode final: public AnimationNode {
public:
	AnimationClipNode(std::shared_ptr<AnimationClip> const& animationClip);

	void Calculate(AnimationTimeInterval const& time,
		Detail::Skeletal2D::AnimationGraphWeightCollection const& weights,
		Skeleton const& skeleton,
		SkeletonPose & skeletonPose) const override;

	AnimationTimeInterval Length() const override;

private:
	std::shared_ptr<AnimationClip> clip;
};

}// namespace Skeletal2D
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONCLIPNODE_6B87052F_753A_4B60_B736_400EBF81491F_HPP)
