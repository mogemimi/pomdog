// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/async/scheduler.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <mutex>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::concurrency {

class POMDOG_EXPORT QueuedScheduler final : public Scheduler {
private:
    using clockType = std::chrono::steady_clock;
    using TimePoint = clockType::time_point;

    struct DeferredTask final {
        std::function<void()> function;
        TimePoint startTime;
    };

    std::vector<DeferredTask> tasks_;
    std::vector<DeferredTask> addedDeferredTasks_;
    std::recursive_mutex addingProtection_;
    std::recursive_mutex tasksProtection_;

public:
    QueuedScheduler();
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
