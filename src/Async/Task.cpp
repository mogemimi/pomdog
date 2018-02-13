// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Async/Task.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <atomic>

namespace Pomdog {
namespace Concurrency {

namespace Detail {

Task<void> WhenAllImpl(const std::vector<Task<void>>& tasks)
{
    if (tasks.empty()) {
        TaskCompletionSource<void> tcs;
        tcs.SetResult();
        Task<void> task(std::move(tcs));
        return task;
    }

    POMDOG_ASSERT(!tasks.empty());

    TaskCompletionSource<void> tcs;
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
    Task<void> task(std::move(tcs));
    return task;
}

} // namespace Detail

} // namespace Concurrency
} // namespace Pomdog
