//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_TEMPORALACTION_DD16B93C_A710_4BD1_914B_5E1AF09287E9_HPP
#define POMDOG_TEMPORALACTION_DD16B93C_A710_4BD1_914B_5E1AF09287E9_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog.Experimental/Actions/Action.hpp"

namespace Pomdog {
namespace Details {
namespace Actions {

class InterpolationLinear {
public:
	float Apply(float normalizedTime)
	{
		return normalizedTime;
	}
};

template <typename ActionClass>
class TemporalAction: public Action {
private:
	AnimationTimeInterval const duration;
	AnimationTimeInterval time;
	bool isCompleted;
	bool began;

	ActionClass internalAction;
	InterpolationLinear interpolation;

public:
	template <typename...Args>
	TemporalAction(AnimationTimeInterval const& durationIn, Args &&...args)
		: duration(durationIn)
		, time(AnimationTimeInterval::zero())
		, isCompleted(false)
		, began(false)
		, internalAction(std::forward<Args>(args)...)
	{}

	void Act(GameObject & gameObject, AnimationTimeInterval const& frameDuration) override
	{
		if (isCompleted) {
			return;
		}

		if (!began) {
			internalAction.Begin(gameObject);
			began = true;
		}

		time += frameDuration;
		float normalizedTime = time.count() / duration.count();

		if (time >= duration) {
			isCompleted = true;
			normalizedTime = 1.f;
		} else {
			normalizedTime = interpolation.Apply(normalizedTime);
		}

		internalAction.Update(gameObject, normalizedTime);
	}

	bool IsCompleted() const override
	{
		return isCompleted;
	}

	AnimationTimeInterval Time() const
	{
		return time;
	}

	AnimationTimeInterval Duration() const
	{
		return duration;
	}
};

}// namespace Actions
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_TEMPORALACTION_DD16B93C_A710_4BD1_914B_5E1AF09287E9_HPP)
