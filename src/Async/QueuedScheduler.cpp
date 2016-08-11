// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog/Async/QueuedScheduler.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>

namespace Pomdog {
namespace Concurrency {

void QueuedScheduler::Schedule(
    std::function<void()> && task,
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
    for (auto & task : tasks) {
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

    for (auto & deferred : temporaryTasks) {
        auto iter = std::lower_bound(
            std::begin(tasks), std::end(tasks), deferred);

        if (iter == std::end(tasks)) {
            tasks.push_back(std::move(deferred));
        }
        else {
            tasks.insert(iter, std::move(deferred));
        }
    }

    POMDOG_ASSERT(std::is_sorted(std::begin(tasks), std::end(tasks)));
}

} // namespace Concurrency
} // namespace Pomdog
