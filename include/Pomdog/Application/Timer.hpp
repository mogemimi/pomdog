// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TIMER_CAC74AF2_HPP
#define POMDOG_TIMER_CAC74AF2_HPP

#include "Pomdog/Application/Duration.hpp"
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
    void Reset();

    bool Enabled() const;

    Duration TotalTime() const;
    Duration FrameDuration() const;

    void Scale(float scale);
    float Scale() const;

private:
    ScopedConnection connection;
    Duration totalTime;
    Duration frameDuration;
    float scale;
    bool  enabled;
};

}// namespace Pomdog

#endif // POMDOG_TIMER_CAC74AF2_HPP
