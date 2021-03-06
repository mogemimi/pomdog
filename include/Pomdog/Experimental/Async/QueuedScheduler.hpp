// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/Async/Scheduler.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <mutex>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Concurrency {

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

} // namespace Pomdog::Concurrency
