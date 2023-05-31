// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/chrono/timer.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/game_clock.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Timer::Timer(GameClock& clock)
    : totalTime_(Duration::zero())
    , scale_(1)
    , enabled_(true)
    , singleShot_(false)
{
    auto onTick = [this](const Duration& frameDurationIn) {
        if (!enabled_) {
            return;
        }
        totalTime_ += (frameDurationIn * scale_);

        if (interval_ && (totalTime_ >= *interval_)) {
            totalTime_ = *interval_;

            elapsed();

            if (singleShot_) {
                enabled_ = false;
            }
            else {
                totalTime_ = Duration::zero();
            }
        }
    };
    connection_ = clock.onTick.Connect(std::move(onTick));
}

Timer::Timer(const std::shared_ptr<GameClock>& clock)
    : Timer(*clock)
{
    POMDOG_ASSERT(clock);
}

Timer::~Timer() = default;

void Timer::start()
{
    enabled_ = true;
}

void Timer::stop()
{
    enabled_ = false;
}

void Timer::reset()
{
    totalTime_ = Duration::zero();
}

bool Timer::isEnabled() const
{
    return enabled_;
}

Duration Timer::getTotalTime() const
{
    return totalTime_;
}

bool Timer::isSingleShot() const
{
    return singleShot_;
}

void Timer::setSingleShot(bool singleShotIn)
{
    singleShot_ = singleShotIn;
}

void Timer::setScale(double scaleIn)
{
    scale_ = scaleIn;
}

double Timer::getScale() const
{
    return scale_;
}

std::optional<Duration> Timer::getInterval() const
{
    return interval_;
}

void Timer::setInterval(const Duration& intervalIn)
{
    interval_ = intervalIn;
}

void Timer::setInterval()
{
    interval_ = std::nullopt;
}

} // namespace pomdog
