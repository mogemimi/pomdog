// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <memory>

namespace Pomdog {

class GameClock;

class POMDOG_EXPORT Timer {
public:
    explicit Timer(GameClock & clock);

    explicit Timer(const std::shared_ptr<GameClock>& clock);

    Timer() = delete;
    Timer(const Timer&) = delete;
    Timer(Timer &&) = default;
    Timer & operator=(const Timer&) = delete;
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

    void SetInterval(const Duration& interval);

    void SetInterval();

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
