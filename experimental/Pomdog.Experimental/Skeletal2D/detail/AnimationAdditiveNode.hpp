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

#include "Pomdog.Experimental/Skeletal2D/AnimationNode.hpp"
#include <memory>

namespace Pomdog {
namespace Details {
namespace Skeletal2D {

class AnimationAdditiveNode final: public AnimationNode {
public:
	AnimationAdditiveNode(std::unique_ptr<AnimationNode> && blendNode1In,
		std::unique_ptr<AnimationNode> && blendNode2In);

	void Calculate(AnimationTimeInterval const& time,
		Details::Skeletal2D::AnimationGraphWeightCollection const& weights,
		Skeleton const& skeleton,
		SkeletonPose & skeletonPose) const override;

	float Weight() const;
	void Weight(float amount);

	AnimationTimeInterval Length() const override;

	std::unique_ptr<AnimationNode> const& Base() const { return base; }
	std::unique_ptr<AnimationNode> const& Additive() const { return additive; }

private:
	std::unique_ptr<AnimationNode> base;
	std::unique_ptr<AnimationNode> additive;
	AnimationTimeInterval length;
	float weight;
};

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONADDITIVENODE_48896792_129A_489D_B3F3_55D3814CFD38_HPP)
