//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_TIMER_CAC74AF2_EF38_48B5_93D0_673E21E66979_HPP
#define POMDOG_TIMER_CAC74AF2_EF38_48B5_93D0_673E21E66979_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "DurationSeconds.hpp"
#include "Pomdog/Event/ScopedConnection.hpp"
#include <memory>

namespace Pomdog {

class GameClock;

class Timer {
public:
	explicit Timer(GameClock & clock);
	
	Timer() = delete;
	Timer(Timer const&) = delete;
	Timer(Timer &&) = default;
	Timer & operator=(Timer const&) = delete;
	Timer & operator=(Timer &&) = default;
	
	~Timer();

	void Start();
	void Stop();
	
	bool Enabled() const;

	DurationSeconds TotalTime() const;
	DurationSeconds FrameDuration() const;

	void Scale(float scale);
	float Scale() const;
	
private:
	ScopedConnection connection;
	DurationSeconds totalTime;
	DurationSeconds frameDuration;
	float scale;
	bool  enabled;
};

}// namespace Pomdog

#endif // !defined(POMDOG_TIMER_CAC74AF2_EF38_48B5_93D0_673E21E66979_HPP)
