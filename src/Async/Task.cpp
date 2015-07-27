// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Async/Task.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <atomic>

namespace Pomdog {
namespace Concurrency {
namespace Detail {
namespace {

struct WhenAnyPromise {
    std::atomic<bool> IsAnyTaskComplete;
};

struct WhenAllPromise {
    std::atomic<int> Count;
    std::atomic<bool> IsRejected;
};

} // unnamed namespace
} // namespace Detail
//-----------------------------------------------------------------------
std::shared_ptr<Scheduler> TaskFactory::GetScheduler() const noexcept
{
    POMDOG_ASSERT(ambientScheduler);
    return ambientScheduler;
}
//-----------------------------------------------------------------------
Task<void> TaskFactory::FromResult() const
{
    POMDOG_ASSERT(ambientScheduler);
    auto scheduler = ambientScheduler;
    POMDOG_ASSERT(scheduler);
    auto tcs = std::make_shared<TaskCompletionSource<void>>(scheduler);
    tcs->SetResult();
    return tcs->GetTask();
}
//-----------------------------------------------------------------------
Task<void> TaskFactory::Delay(Duration const& delay) const
{
    POMDOG_ASSERT(ambientScheduler);
    return Delay(delay, ambientScheduler);
}
//-----------------------------------------------------------------------
Task<void> TaskFactory::Delay(
    Duration const& delay,
    std::shared_ptr<Scheduler> const& scheduler)
{
    POMDOG_ASSERT(scheduler);
    auto tcs = std::make_shared<TaskCompletionSource<void>>(scheduler);
    scheduler->Schedule([tcs] {
        tcs->SetResult();
    }, delay);
    return tcs->GetTask();
}
//-----------------------------------------------------------------------
Task<void> TaskFactory::WhenAny(std::vector<Task<void>> && tasks) const
{
    POMDOG_ASSERT(ambientScheduler);
    return WhenAny(std::move(tasks), ambientScheduler);
}
//-----------------------------------------------------------------------
Task<void> TaskFactory::WhenAny(
    std::vector<Task<void>> && tasks,
    std::shared_ptr<Scheduler> const& scheduler)
{
    if (tasks.empty()) {
        auto tcs = std::make_shared<TaskCompletionSource<void>>(scheduler);
        tcs->SetResult();
        return tcs->GetTask();
    }

    POMDOG_ASSERT(!tasks.empty());
    POMDOG_ASSERT(scheduler);

    auto tcs = std::make_shared<TaskCompletionSource<void>>(scheduler);
    auto whenAnyPromise = std::make_shared<Detail::WhenAnyPromise>();
    whenAnyPromise->IsAnyTaskComplete.store(false);

    for (auto & task : tasks) {
        task.Then([tcs, whenAnyPromise] {
            if (!whenAnyPromise->IsAnyTaskComplete.load()) {
                whenAnyPromise->IsAnyTaskComplete.store(true);
                tcs->SetResult();
            }
            POMDOG_ASSERT(whenAnyPromise->IsAnyTaskComplete.load());
        });
        task.Catch([tcs, whenAnyPromise](std::exception_ptr const& ex) {
            if (!whenAnyPromise->IsAnyTaskComplete.load()) {
                whenAnyPromise->IsAnyTaskComplete.store(true);
                tcs->SetException(ex);
            }
            POMDOG_ASSERT(whenAnyPromise->IsAnyTaskComplete.load());
        });
    }
    return tcs->GetTask();
}
//-----------------------------------------------------------------------
Task<void> TaskFactory::WhenAll(std::vector<Task<void>> && tasks) const
{
    POMDOG_ASSERT(ambientScheduler);
    return WhenAll(std::move(tasks), ambientScheduler);
}
//-----------------------------------------------------------------------
Task<void> TaskFactory::WhenAll(
    std::vector<Task<void>> && tasks,
    std::shared_ptr<Scheduler> const& scheduler)
{
    if (tasks.empty()) {
        auto tcs = std::make_shared<TaskCompletionSource<void>>(scheduler);
        tcs->SetResult();
        return tcs->GetTask();
    }

    POMDOG_ASSERT(!tasks.empty());
    POMDOG_ASSERT(scheduler);

    auto tcs = std::make_shared<TaskCompletionSource<void>>(scheduler);
    auto whenAllPromise = std::make_shared<Detail::WhenAllPromise>();
    whenAllPromise->Count.store(static_cast<int>(tasks.size()));
    whenAllPromise->IsRejected.store(false);

    for (auto & task : tasks) {
        task.Then([tcs, whenAllPromise] {
            if (whenAllPromise->IsRejected.load()) {
                return;
            }
            --whenAllPromise->Count;
            if (whenAllPromise->Count.load() <= 0) {
                tcs->SetResult();
            }
            POMDOG_ASSERT(whenAllPromise->Count >= 0);
        });
        task.Catch([tcs, whenAllPromise](std::exception_ptr const& ex) {
            if (!whenAllPromise->IsRejected.load()) {
                whenAllPromise->IsRejected.store(true);
                tcs->SetException(ex);
            }
        });
    }
    return tcs->GetTask();
}
//-----------------------------------------------------------------------
TaskCompletionSource<void>::TaskCompletionSource(Task<void> const& taskIn)
    : task(taskIn)
{
    POMDOG_ASSERT(task.body);
}
//-----------------------------------------------------------------------
TaskCompletionSource<void>::TaskCompletionSource(std::shared_ptr<Scheduler> const& scheduler)
    : task(scheduler)
{
}
//-----------------------------------------------------------------------
Task<void> TaskCompletionSource<void>::GetTask() const
{
    return task;
}
//-----------------------------------------------------------------------
void TaskCompletionSource<void>::SetResult()
{
    POMDOG_ASSERT(task.body);
    std::lock_guard<std::mutex> lock(task.body->mutex);
    POMDOG_ASSERT(!task.body->ranToCompletion);
    if (task.body->ranToCompletion) {
        throw std::runtime_error("future error");
    }
    task.body->ranToCompletion = true;
    task.body->RunContinuations();
}
//-----------------------------------------------------------------------
void TaskCompletionSource<void>::SetException(std::exception_ptr const& exception)
{
    POMDOG_ASSERT(task.body);
    std::lock_guard<std::mutex> lock(task.body->mutex);
    POMDOG_ASSERT(!task.body->ranToCompletion);
    if (task.body->ranToCompletion) {
        throw std::runtime_error("future error");
    }
    task.body->ranToCompletion = true;
    task.body->exceptionPointer = exception;
    task.body->RunContinuations();
}
//-----------------------------------------------------------------------
} // namespace Concurrency
} // namespace Pomdog
