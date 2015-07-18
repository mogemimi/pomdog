// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Async/Scheduler.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <vector>
#include <utility>
#include <mutex>

namespace Pomdog {
namespace Concurrency {
namespace {

class TaskIndexCounter {
public:
    TaskIndexCounter()
        : threadSafeCounter(0)
    {}

    std::uint32_t GetNext()
    {
        ++threadSafeCounter;
        return threadSafeCounter.load();
    }

private:
    std::atomic<std::uint32_t> threadSafeCounter;
};

template <typename TimePoint>
struct DeferredTask {
    std::function<void()> Action;
    TimePoint StartTime;
    std::uint32_t Id;
};

template <typename TimePoint>
static bool Compare(DeferredTask<TimePoint> const& a, DeferredTask<TimePoint> const& b)
{
    if (a.StartTime == b.StartTime) {
        return a.Id < b.Id;
    }
    return a.StartTime < b.StartTime;
}

} // unnamed namespace
//-----------------------------------------------------------------------
class TickedScheduler::Impl {
public:
    typedef std::chrono::steady_clock clockType;
    typedef clockType::time_point TimePoint;
    typedef std::function<void()> Function;
    std::vector<DeferredTask<TimePoint>> deferredTasks;
    std::vector<DeferredTask<TimePoint>> addedDeferredTasks;
    TaskIndexCounter counter;
    std::recursive_mutex addingProtection;
    std::recursive_mutex tasksProtection;

public:
    template <typename TFunction>
    void QueueTask(TFunction && command, Duration const& delay);

    void Tick();

private:
    void MergeTasks();
};
//-----------------------------------------------------------------------
template <typename TFunction>
void TickedScheduler::Impl::QueueTask(TFunction && command, Duration const& delay)
{
    POMDOG_ASSERT(command);

    DeferredTask<TimePoint> deferred;
    deferred.Action = std::forward<TFunction>(command);
    deferred.StartTime = clockType::now() + std::chrono::duration_cast<clockType::duration>(delay);
    deferred.Id = counter.GetNext();

    {
        std::lock_guard<std::recursive_mutex> lock(addingProtection);
        addedDeferredTasks.push_back(std::move(deferred));
    }
}
//-----------------------------------------------------------------------
void TickedScheduler::Impl::MergeTasks()
{
    decltype(addedDeferredTasks) temporaryTasks;
    {
        std::lock_guard<std::recursive_mutex> lock(addingProtection);
        std::swap(addedDeferredTasks, temporaryTasks);
    }

    std::lock_guard<std::recursive_mutex> lock(tasksProtection);

    for (auto & deferred : temporaryTasks) {
        // binary search
        auto iter = std::lower_bound(
            std::begin(deferredTasks), std::end(deferredTasks),
            deferred, Compare<TimePoint>);

        if (iter == std::end(deferredTasks)) {
            deferredTasks.push_back(std::move(deferred));
        }
        else {
            deferredTasks.insert(iter, std::move(deferred));
        }
    }

    POMDOG_ASSERT(std::is_sorted(
        std::begin(deferredTasks), std::end(deferredTasks), Compare<TimePoint>));
}
//-----------------------------------------------------------------------
void TickedScheduler::Impl::Tick()
{
    MergeTasks();

    bool needToErace = false;
    const auto currentTime = clockType::now();

    std::lock_guard<std::recursive_mutex> lock(tasksProtection);

    auto taskIter = std::begin(deferredTasks);
    for (auto & task : deferredTasks) {
        if (task.StartTime <= currentTime) {
            POMDOG_ASSERT(task.Action);
            task.Action();
            needToErace = true;
            POMDOG_ASSERT(taskIter->Id == task.Id);
            ++taskIter;
        }
        else {
            POMDOG_ASSERT(task.StartTime > currentTime);
            break;
        }
    }

    if (needToErace) {
        deferredTasks.erase(std::begin(deferredTasks), taskIter);
    }
}
//-----------------------------------------------------------------------
TickedScheduler::TickedScheduler()
    : impl(std::make_unique<Impl>())
{
}
//-----------------------------------------------------------------------
TickedScheduler::~TickedScheduler() = default;
//-----------------------------------------------------------------------
void TickedScheduler::Schedule(std::function<void()> && task)
{
    POMDOG_ASSERT(impl);
    impl->QueueTask(std::move(task), Duration::zero());
}
//-----------------------------------------------------------------------
void TickedScheduler::Schedule(std::function<void()> && task, Duration const& delay)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(delay.count() >= 0);
    impl->QueueTask(std::move(task), delay);
}
//-----------------------------------------------------------------------
void TickedScheduler::Schedule(std::function<void()> const& task)
{
    POMDOG_ASSERT(impl);
    impl->QueueTask(task, Duration::zero());
}
//-----------------------------------------------------------------------
void TickedScheduler::Schedule(std::function<void()> const& task, Duration const& delay)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(delay.count() >= 0);
    impl->QueueTask(task, delay);
}
//-----------------------------------------------------------------------
void TickedScheduler::Tick()
{
    POMDOG_ASSERT(impl);
    impl->Tick();
}
//-----------------------------------------------------------------------
void ImmediateExecutor::Schedule(std::function<void()> && task)
{
    POMDOG_ASSERT(task);
    task();
}
//-----------------------------------------------------------------------
void ImmediateExecutor::Schedule(std::function<void()> const& task)
{
    POMDOG_ASSERT(task);
    task();
}
//-----------------------------------------------------------------------
void ImmediateExecutor::Schedule(std::function<void()> && task, Duration const&)
{
    POMDOG_ASSERT(task);
    task();
}
//-----------------------------------------------------------------------
void ImmediateExecutor::Schedule(std::function<void()> const& task, Duration const&)
{
    POMDOG_ASSERT(task);
    task();
}
//-----------------------------------------------------------------------
} // namespace Concurrency
} // namespace Pomdog
