//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ANIMATIONTIMER_A9EAE898_56A1_4928_B031_22D9304214E6_HPP
#define POMDOG_ANIMATIONTIMER_A9EAE898_56A1_4928_B031_22D9304214E6_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Application/DurationSeconds.hpp>

namespace Pomdog {

class AnimationTimer {
public:
	AnimationTimer();

	void Update(DurationSeconds const& time);
	void Pause();
	void Continue();
	void Reset();

	DurationSeconds Time() const;
	bool IsPause() const;
	
private:
	DurationSeconds time;
	bool isPause;
};

}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONTIMER_A9EAE898_56A1_4928_B031_22D9304214E6_HPP)
