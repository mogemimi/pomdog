// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/async/queued_scheduler.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::concurrency {
namespace {

template <class TDeferredTask>
bool Compare(const TDeferredTask& a, const TDeferredTask& b) noexcept
{
    return a.StartTime < b.StartTime;
}

} // namespace

void QueuedScheduler::Schedule(
    std::function<void()>&& task,
    const Duration& delayTime)
{
    POMDOG_ASSERT(task);
    POMDOG_ASSERT(delayTime >= std::chrono::duration<double>::zero());

    DeferredTask deferred;
    deferred.Function = std::move(task);
    deferred.StartTime = clockType::now() +
        std::chrono::duration_cast<clockType::duration>(delayTime);

    {
        std::lock_guard<std::recursive_mutex> lock(addingProtection);
        addedDeferredTasks.push_back(std::move(deferred));
    }
}

void QueuedScheduler::Update()
{
    MergeTasks();

    bool needToErace = false;
    const auto currentTime = clockType::now();

    std::lock_guard<std::recursive_mutex> lock(tasksProtection);

    auto completedTask = std::begin(tasks);
    for (auto& task : tasks) {
        if (task.StartTime <= currentTime) {
            POMDOG_ASSERT(task.Function);
            task.Function();
            needToErace = true;
            POMDOG_ASSERT(completedTask != std::end(tasks));
            ++completedTask;
        }
        else {
            POMDOG_ASSERT(task.StartTime > currentTime);
            break;
        }
    }

    if (needToErace) {
        tasks.erase(std::begin(tasks), completedTask);
    }
}

bool QueuedScheduler::Empty() noexcept
{
    {
        std::lock_guard<std::recursive_mutex> lock(addingProtection);
        if (!addedDeferredTasks.empty()) {
            return false;
        }
    }
    std::lock_guard<std::recursive_mutex> lock(tasksProtection);
    return tasks.empty();
}

void QueuedScheduler::MergeTasks()
{
    decltype(addedDeferredTasks) temporaryTasks;
    {
        std::lock_guard<std::recursive_mutex> lock(addingProtection);
        std::swap(addedDeferredTasks, temporaryTasks);
    }

    std::lock_guard<std::recursive_mutex> lock(tasksProtection);

    for (auto& deferred : temporaryTasks) {
        auto iter = std::lower_bound(
            std::begin(tasks), std::end(tasks), deferred, Compare<DeferredTask>);

        if (iter == std::end(tasks)) {
            tasks.push_back(std::move(deferred));
        }
        else {
            tasks.insert(iter, std::move(deferred));
        }
    }

    POMDOG_ASSERT(std::is_sorted(std::begin(tasks), std::end(tasks), Compare<DeferredTask>));
}

} // namespace pomdog::concurrency
