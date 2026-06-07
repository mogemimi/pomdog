// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/chrono/time_point.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

/// FrameRateLimiter paces a game loop to a target frame rate using a
/// "debt-discarding" cumulative deadline. Each frame targets the next point on
/// a fixed time grid (anchor + k * frameDuration), so short-term timing errors
/// do not move the grid. When a frame runs long, missed grid slots are skipped
/// rather than repaid, which avoids bursts of zero-sleep catch-up frames; the
/// lost time is not made up, so the average rate dips during a hiccup and then
/// resumes on the next valid grid point.
///
/// It is mechanism-agnostic: it only computes how long the caller should sleep;
/// the caller performs the actual wait (e.g. via the Win32 PreciseSleeper or
/// `std::this_thread::sleep_for`). The frame-rate cap is independent of the
/// V-Sync mode; see `GameHost::setMaxFramesPerSecond` for their intended
/// interaction.
class FrameRateLimiter final {
private:
    // NOTE: The target seconds-per-frame, or nullopt when limiting is disabled.
    std::optional<Duration> frameDuration_;

    // NOTE: The target completion time of the current frame.
    TimePoint deadline_ = {};

    // NOTE: True until the deadline is anchored to a real timestamp. Set on
    // construction, on disable(), and whenever the target frame rate changes.
    bool needsReanchor_ = true;

public:
    /// Sets the target frame rate in frames per second. A non-positive value
    /// disables limiting. The cumulative deadline is re-anchored on the next
    /// `computeSleepDuration()` call, so changing the cap at runtime does not
    /// produce a transient burst of catch-up frames.
    void
    setTargetFrameRate(i32 framesPerSecond) noexcept;

    /// Disables frame-rate limiting. `computeSleepDuration()` then returns zero.
    void
    disable() noexcept;

    /// Returns true if a target frame rate is set.
    [[nodiscard]] bool
    isEnabled() const noexcept;

    /// Advances to the next frame deadline and returns how long to sleep from
    /// `now` to reach it (at most one frame duration). Returns
    /// `Duration::zero()` when limiting is disabled.
    ///
    /// If that deadline is already in the past, the missed grid slots are skipped
    /// so the stored deadline lands on the next grid point strictly after `now`,
    /// rather than producing a burst of zero-sleep catch-up frames. This also
    /// absorbs large stalls (debugger pause, OS sleep/resume, GPU stall, window
    /// drag, ...). Keeping the deadline close to `now` makes floating-point
    /// accumulation error and overflow of the f64-seconds `TimePoint` irrelevant
    /// in practice.
    [[nodiscard]] Duration
    computeSleepDuration(TimePoint now) noexcept;
};

} // namespace pomdog::detail
