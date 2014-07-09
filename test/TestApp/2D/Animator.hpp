//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ANIMATOR_0CFC0E37_CCD5_481A_9275_E8579887ACB0_HPP
#define POMDOG_ANIMATOR_0CFC0E37_CCD5_481A_9275_E8579887ACB0_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Application/GameClock.hpp>
#include <Pomdog/Gameplay/Component.hpp>

namespace Pomdog {

class Animator: public Component<Animator> {
public:
	virtual ~Animator() = default;
	
	virtual void Update(GameClock const& clock) = 0;
	
	virtual float PlaybackRate() const = 0;
	virtual void PlaybackRate(float playbackRate) = 0;
	
	virtual void SetFloat(std::string const& name, float value) = 0;
	virtual void SetBool(std::string const& name, bool value) = 0;
};

}// namespace Pomdog

#include <memory>
#include <Pomdog/Pomdog.hpp>
#include "../Skeletal2D/Skeleton.hpp"
#include "../Skeletal2D/SkeletonTransform.hpp"
#include "../Skeletal2D/AnimationNode.hpp"
#include "../Skeletal2D/AnimationState.hpp"
#include "../Skeletal2D/AnimationGraphWeightCollection.hpp"
#include "../Skeletal2D/AnimationGraph.hpp"

namespace Pomdog {

class MaidAnimator: public Animator {
public:
	MaidAnimator(std::shared_ptr<Skeleton> const& skeleton,
		std::shared_ptr<SkeletonTransform> const& skeletonTransform,
		std::shared_ptr<AnimationGraph> const& animationGraph);
	
	void Update(GameClock const& clock) override;
	
	float PlaybackRate() const override;
	
	void PlaybackRate(float playbackRate) override;
	
	void SetFloat(std::string const& name, float value) override;

	void SetBool(std::string const& name, bool value) override;
	
private:
	AnimationGraphWeightCollection graphWeights;
	std::shared_ptr<Skeleton> skeleton;
	std::shared_ptr<SkeletonTransform> skeletonTransform;
	std::shared_ptr<AnimationGraph> animationGraph;
	AnimationTimeInterval time;
	float playbackRate;
	
};

}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATOR_0CFC0E37_CCD5_481A_9275_E8579887ACB0_HPP)
