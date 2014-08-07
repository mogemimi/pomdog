//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ANIMATIONLERPNODE_22AAFE06_B82E_49EC_8B16_EC01D32BC836_HPP
#define POMDOG_ANIMATIONLERPNODE_22AAFE06_B82E_49EC_8B16_EC01D32BC836_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "AnimationNode.hpp"
#include <memory>

namespace Pomdog {

class AnimationLerpNode final: public AnimationNode {
public:
	AnimationLerpNode(std::unique_ptr<AnimationNode> && blendNode1,
		std::unique_ptr<AnimationNode> && blendNode2, std::uint16_t weightIndex);

	void Calculate(AnimationTimeInterval const& time,
		AnimationGraphWeightCollection const& weights, Skeleton const& skeleton, SkeletonPose & skeletonPose) override;

	AnimationTimeInterval Length() const override;

	std::unique_ptr<AnimationNode> const& A() const;
	std::unique_ptr<AnimationNode> const& B() const;

private:
	std::unique_ptr<AnimationNode> nodeA;
	std::unique_ptr<AnimationNode> nodeB;
	AnimationTimeInterval length;
	std::uint16_t weightIndex;
};

}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONLERPNODE_22AAFE06_B82E_49EC_8B16_EC01D32BC836_HPP)
