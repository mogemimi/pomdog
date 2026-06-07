// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/backends/frame_rate_limiter.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

void FrameRateLimiter::setTargetFrameRate(i32 framesPerSecond) noexcept
{
    if (framesPerSecond <= 0) {
        disable();
        return;
    }
    frameDuration_ = Duration{1.0} / static_cast<f64>(framesPerSecond);

    // NOTE: Re-anchor on the next tick so a changed cap takes effect cleanly
    // instead of carrying over a now-stale deadline.
    needsReanchor_ = true;
}

void FrameRateLimiter::disable() noexcept
{
    frameDuration_ = std::nullopt;
    needsReanchor_ = true;
}

bool FrameRateLimiter::isEnabled() const noexcept
{
    return frameDuration_.has_value();
}

Duration
FrameRateLimiter::computeSleepDuration(TimePoint now) noexcept
{
    if (!frameDuration_.has_value()) {
        return Duration::zero();
    }
    const Duration frame = *frameDuration_;

    if (needsReanchor_) {
        // NOTE: First frame after enable / target change. Anchor the grid at
        // this frame boundary.
        deadline_ = now;
        needsReanchor_ = false;
    }

    // NOTE: Advance to this frame's slot on the fixed grid (anchor + k*frame).
    // The subtraction from `now` below shortens the sleep by the time already
    // spent in the frame.
    deadline_ += frame;

    if (const Duration behind = now - deadline_; behind > Duration::zero()) {
        // NOTE: The frame ran long enough that this slot is already in the past.
        // Debt-discarding: skip the missed whole slots and jump to the next grid
        // point strictly after `now`, instead of emitting a burst of zero-sleep
        // catch-up frames. The grid phase is preserved; the lost time is not made
        // up. This single rule also absorbs large stalls (debugger pause, OS
        // sleep/resume, GPU stall, window drag).
        const f64 slotsToSkip = std::floor(behind / frame) + 1.0;
        deadline_ += frame * slotsToSkip;
    }

    const Duration sleepDuration = deadline_ - now;
    return std::max(sleepDuration, Duration::zero());
}

} // namespace pomdog::detail
