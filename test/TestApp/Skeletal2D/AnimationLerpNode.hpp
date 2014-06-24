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

#include <memory>
#include "AnimationNode.hpp"

namespace Pomdog {

class AnimationLerpNode final: public AnimationNode {
public:
	AnimationLerpNode(std::unique_ptr<AnimationNode> && blendNode1In,
		std::unique_ptr<AnimationNode> && blendNode2In);

	void Calculate(AnimationTimeInterval const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose) override;

	float Weight() const;
	void Weight(float amount);

	std::unique_ptr<AnimationNode> const& A() const { return nodeA; }
	std::unique_ptr<AnimationNode> const& B() const { return nodeB; }

private:
	std::unique_ptr<AnimationNode> nodeA;
	std::unique_ptr<AnimationNode> nodeB;
	float weight;
};

}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONLERPNODE_22AAFE06_B82E_49EC_8B16_EC01D32BC836_HPP)
