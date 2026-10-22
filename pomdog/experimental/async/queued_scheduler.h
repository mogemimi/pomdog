// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/time_point.h"
#include "pomdog/experimental/async/scheduler.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <mutex>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
class TimeSource;
} // namespace pomdog::detail

namespace pomdog::concurrency {

class POMDOG_EXPORT QueuedScheduler final : public Scheduler {
private:
    struct DeferredTask final {
        std::function<void()> function;
        TimePoint startTime;
    };

    std::shared_ptr<pomdog::detail::TimeSource> timeSource_;
    std::vector<DeferredTask> tasks_;
    std::vector<DeferredTask> addedDeferredTasks_;
    std::recursive_mutex addingProtection_;
    std::recursive_mutex tasksProtection_;

public:
    /// Creates a scheduler that reads the current time from the platform time source.
    QueuedScheduler();

    /// Creates a scheduler that reads the current time from timeSource.
    /// Tests pass a fake time source to control when tasks become due.
    explicit QueuedScheduler(const std::shared_ptr<pomdog::detail::TimeSource>& timeSource);

    QueuedScheduler(const QueuedScheduler&) = delete;
    QueuedScheduler& operator=(const QueuedScheduler&) = delete;

    void schedule(
        std::function<void()>&& task,
        const Duration& delayTime = Duration::zero()) override;

    void update();

    [[nodiscard]] bool empty() noexcept;

private:
    void mergeTasks();
};

} // namespace pomdog::concurrency
