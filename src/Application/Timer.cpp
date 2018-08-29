// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Application/Timer.hpp"
#include "Pomdog/Application/GameClock.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {

Timer::Timer(GameClock & clock)
    : totalTime(Duration::zero())
    , scale(1)
    , enabled(true)
    , isSingleShot(false)
{
    auto onTick = [this](const Duration& frameDurationIn) {
        if (!enabled) {
            return;
        }
        totalTime += (frameDurationIn * scale);

        if (interval && (totalTime >= *interval)) {
            totalTime = *interval;

            this->Elapsed();

            if (isSingleShot) {
                enabled = false;
            }
            else {
                totalTime = Duration::zero();
            }
        }
    };
    connection = clock.OnTick.Connect(std::move(onTick));
}

Timer::Timer(const std::shared_ptr<GameClock>& clock)
    : Timer(*clock)
{
    POMDOG_ASSERT(clock);
}

Timer::~Timer() = default;

void Timer::Start()
{
    this->enabled = true;
}

void Timer::Stop()
{
    this->enabled = false;
}

void Timer::Reset()
{
    totalTime = Duration::zero();
}

bool Timer::IsEnabled() const
{
    return this->enabled;
}

Duration Timer::GetTotalTime() const
{
    return this->totalTime;
}

bool Timer::IsSingleShot() const
{
    return this->isSingleShot;
}

void Timer::SetSingleShot(bool isSingleShotIn)
{
    this->isSingleShot = isSingleShotIn;
}

void Timer::SetScale(double scaleIn)
{
    this->scale = scaleIn;
}

double Timer::GetScale() const
{
    return this->scale;
}

std::optional<Duration> Timer::GetInterval() const
{
    return this->interval;
}

void Timer::SetInterval(const Duration& intervalIn)
{
    this->interval = intervalIn;
}

void Timer::SetInterval()
{
    this->interval = std::nullopt;
}

} // namespace Pomdog
