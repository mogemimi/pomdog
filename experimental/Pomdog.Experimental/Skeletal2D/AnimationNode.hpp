//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ANIMATIONNODE_DEEDC3FD_AB3D_4C6B_AC05_E5E22457E878_HPP
#define POMDOG_ANIMATIONNODE_DEEDC3FD_AB3D_4C6B_AC05_E5E22457E878_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "AnimationTimeInterval.hpp"

namespace Pomdog {

class AnimationGraphWeightCollection;
class Skeleton;
class SkeletonPose;

class AnimationNode {
public:
	virtual ~AnimationNode() = default;
	
	virtual void Calculate(AnimationTimeInterval const& time,
		AnimationGraphWeightCollection const& weights, Skeleton const& skeleton, SkeletonPose & skeletonPose) = 0;
	
	virtual AnimationTimeInterval Length() const = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONNODE_DEEDC3FD_AB3D_4C6B_AC05_E5E22457E878_HPP)
