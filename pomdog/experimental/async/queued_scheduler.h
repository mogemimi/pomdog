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

    struct DeferredTask {
        std::function<void()> Function;
        TimePoint StartTime;
    };

    std::vector<DeferredTask> tasks;
    std::vector<DeferredTask> addedDeferredTasks;
    std::recursive_mutex addingProtection;
    std::recursive_mutex tasksProtection;

public:
    void Schedule(
        std::function<void()>&& task,
        const Duration& delayTime = Duration::zero()) override;

    void Update();

    bool Empty() noexcept;

private:
    void MergeTasks();
};

} // namespace pomdog::concurrency
