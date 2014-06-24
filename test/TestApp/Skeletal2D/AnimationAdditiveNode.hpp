//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ANIMATIONADDITIVENODE_48896792_129A_489D_B3F3_55D3814CFD38_HPP
#define POMDOG_ANIMATIONADDITIVENODE_48896792_129A_489D_B3F3_55D3814CFD38_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include "AnimationNode.hpp"

namespace Pomdog {

class AnimationAdditiveNode final: public AnimationNode {
public:
	AnimationAdditiveNode(std::unique_ptr<AnimationNode> && blendNode1In,
		std::unique_ptr<AnimationNode> && blendNode2In);

	void Calculate(AnimationTimeInterval const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose) override;

	float Weight() const;
	void Weight(float amount);

	std::unique_ptr<AnimationNode> const& Base() const { return base; }
	std::unique_ptr<AnimationNode> const& Additive() const { return additive; }

private:
	std::unique_ptr<AnimationNode> base;
	std::unique_ptr<AnimationNode> additive;
	float weight;
};

}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONADDITIVENODE_48896792_129A_489D_B3F3_55D3814CFD38_HPP)
