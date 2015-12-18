// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_TIMER_CAC74AF2_HPP
#define POMDOG_TIMER_CAC74AF2_HPP

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <memory>

namespace Pomdog {

class GameClock;

class POMDOG_EXPORT Timer {
public:
    explicit Timer(GameClock & clock);

    explicit Timer(std::shared_ptr<GameClock> const& clock);

    Timer() = delete;
    Timer(Timer const&) = delete;
    Timer(Timer &&) = default;
    Timer & operator=(Timer const&) = delete;
    Timer & operator=(Timer &&) = default;

    ~Timer();

    void Start();
    void Stop();
    void Reset();

    bool IsEnabled() const;

    Duration GetTotalTime() const;

    Duration GetFrameDuration() const;

    bool IsSingleShot() const;

    void SetSingleShot(bool isSingleShot);

    double GetScale() const;

    void SetScale(double scale);

    Optional<Duration> GetInterval() const;

    void SetInterval(Duration const& interval);

    void SetInterval(Optional<Duration> const& interval);

    Signal<void()> Elapsed;

private:
    ScopedConnection connection;
    Optional<Duration> interval;
    Duration totalTime;
    Duration frameDuration;
    double scale;
    bool enabled;
    bool isSingleShot;
};

} // namespace Pomdog

#endif // POMDOG_TIMER_CAC74AF2_HPP
