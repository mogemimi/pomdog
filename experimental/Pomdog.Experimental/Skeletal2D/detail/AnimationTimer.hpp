//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_ANIMATIONTIMER_A9EAE898_56A1_4928_B031_22D9304214E6_HPP
#define POMDOG_ANIMATIONTIMER_A9EAE898_56A1_4928_B031_22D9304214E6_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog.Experimental/Skeletal2D/AnimationTimeInterval.hpp"

namespace Pomdog {
namespace Details {
namespace Skeletal2D {

class AnimationTimer {
public:
	AnimationTimer();

	void Update(AnimationTimeInterval const& time);
	void Pause();
	void Resume();
	void Reset();

	AnimationTimeInterval Time() const;
	bool IsPlaying() const;
	
private:
	AnimationTimeInterval time;
	bool isPlaying;
};

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONTIMER_A9EAE898_56A1_4928_B031_22D9304214E6_HPP)
