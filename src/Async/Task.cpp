// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#include "Pomdog/Async/Task.hpp"
#include "Pomdog/Async/ImmediateScheduler.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <atomic>

namespace Pomdog {
namespace Concurrency {
namespace {

static std::shared_ptr<Scheduler> ambientScheduler =
    std::make_shared<ImmediateScheduler>();

} // unnamed namespace

std::shared_ptr<Scheduler> GetAmbientScheduler()
{
    return ambientScheduler;
}

void SetAmbientScheduler(const std::shared_ptr<Scheduler>& scheduler)
{
    ambientScheduler = scheduler;
}

void Get(const Task<void>& task)
{
    task.Wait();
    if (auto execption = Detail::TaskImpl::GetExceptionPointer(task)) {
        std::rethrow_exception(execption);
    }
}

Task<void> Delay(
    const Duration& dueTime,
    const std::shared_ptr<Scheduler>& scheduler)
{
    POMDOG_ASSERT(scheduler);
    TaskCompletionSource<void> tcs(scheduler);
    scheduler->Schedule([tcs] { tcs.SetResult(); }, dueTime);
    return CreateTask(tcs);
}

Task<void> FromResult(
    const std::shared_ptr<Scheduler>& scheduler)
{
    POMDOG_ASSERT(scheduler);
    TaskCompletionSource<void> tcs(scheduler);
    tcs.SetResult();
    return CreateTask(tcs);
}

namespace Detail {

Task<void> WhenAllImpl(
    const std::vector<Task<void>>& tasks,
    const std::shared_ptr<Scheduler>& scheduler)
{
    if (tasks.empty()) {
        return FromResult(scheduler);
    }

    POMDOG_ASSERT(!tasks.empty());
    POMDOG_ASSERT(scheduler);

    TaskCompletionSource<void> tcs(scheduler);
    auto whenAllPromise = std::make_shared<Detail::WhenAllPromise<void>>();
    whenAllPromise->count = static_cast<int>(tasks.size());
    whenAllPromise->isRejected = false;

    for (auto & task : tasks) {
        task.ContinueWith([tcs, whenAllPromise](const Task<void>& t) {
            std::lock_guard<std::mutex> lock(whenAllPromise->mutex);
            if (whenAllPromise->isRejected) {
                return;
            }
            if (t.IsRejected()) {
                whenAllPromise->isRejected = true;
                tcs.SetException(TaskImpl::GetExceptionPointer(t));
            }
            else {
                --whenAllPromise->count;
                POMDOG_ASSERT(whenAllPromise->count >= 0);
                if (whenAllPromise->count <= 0) {
                    tcs.SetResult();
                }
            }
        });
    }
    return CreateTask(tcs);
}

} // namespace Detail

} // namespace Concurrency
} // namespace Pomdog
