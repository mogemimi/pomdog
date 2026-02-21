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
[[nodiscard]] bool
compare(const TDeferredTask& a, const TDeferredTask& b) noexcept
{
    return a.startTime < b.startTime;
}

} // namespace

QueuedScheduler::QueuedScheduler() = default;

void QueuedScheduler::schedule(
    std::function<void()>&& task,
    const Duration& delayTime)
{
    POMDOG_ASSERT(task);
    POMDOG_ASSERT(delayTime >= std::chrono::duration<double>::zero());

    DeferredTask deferred;
    deferred.function = std::move(task);
    deferred.startTime = clockType::now() +
                         std::chrono::duration_cast<clockType::duration>(delayTime);

    {
        std::lock_guard<std::recursive_mutex> lock(addingProtection_);
        addedDeferredTasks_.push_back(std::move(deferred));
    }
}

void QueuedScheduler::update()
{
    mergeTasks();

    bool needToErace = false;
    const auto currentTime = clockType::now();

    std::lock_guard<std::recursive_mutex> lock(tasksProtection_);

    auto completedTask = std::begin(tasks_);
    for (auto& task : tasks_) {
        if (task.startTime <= currentTime) {
            POMDOG_ASSERT(task.function);
            task.function();
            needToErace = true;
            POMDOG_ASSERT(completedTask != std::end(tasks_));
            ++completedTask;
        }
        else {
            POMDOG_ASSERT(task.startTime > currentTime);
            break;
        }
    }

    if (needToErace) {
        tasks_.erase(std::begin(tasks_), completedTask);
    }
}

bool QueuedScheduler::empty() noexcept
{
    {
        std::lock_guard<std::recursive_mutex> lock(addingProtection_);
        if (!addedDeferredTasks_.empty()) {
            return false;
        }
    }
    std::lock_guard<std::recursive_mutex> lock(tasksProtection_);
    return tasks_.empty();
}

void QueuedScheduler::mergeTasks()
{
    decltype(addedDeferredTasks_) temporaryTasks;
    {
        std::lock_guard<std::recursive_mutex> lock(addingProtection_);
        std::swap(addedDeferredTasks_, temporaryTasks);
    }

    std::lock_guard<std::recursive_mutex> lock(tasksProtection_);

    for (auto& deferred : temporaryTasks) {
        auto iter = std::lower_bound(
            std::begin(tasks_), std::end(tasks_), deferred, compare<DeferredTask>);

        if (iter == std::end(tasks_)) {
            tasks_.push_back(std::move(deferred));
        }
        else {
            tasks_.insert(iter, std::move(deferred));
        }
    }

    POMDOG_ASSERT(std::is_sorted(std::begin(tasks_), std::end(tasks_), compare<DeferredTask>));
}

} // namespace pomdog::concurrency
