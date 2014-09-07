//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ANIMATIONCLIPNODE_6B87052F_753A_4B60_B736_400EBF81491F_HPP
#define POMDOG_ANIMATIONCLIPNODE_6B87052F_753A_4B60_B736_400EBF81491F_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "AnimationNode.hpp"
#include <memory>

namespace Pomdog {

class AnimationClip;

class AnimationClipNode final: public AnimationNode {
public:
	AnimationClipNode(std::shared_ptr<AnimationClip> const& animationClip);

	void Calculate(AnimationTimeInterval const& time,
		AnimationGraphWeightCollection const& weights,
		Skeleton const& skeleton,
		SkeletonPose & skeletonPose) const override;

	AnimationTimeInterval Length() const override;

private:
	std::shared_ptr<AnimationClip> clip;
};

}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONCLIPNODE_6B87052F_753A_4B60_B736_400EBF81491F_HPP)
