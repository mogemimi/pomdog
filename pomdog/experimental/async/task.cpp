// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/async/task.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <atomic>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::concurrency::detail {

void TaskImpl::InnerSetResult(
    const TaskCompletionSource<void>& tcs,
    const TaskResult<void>&)
{
    tcs.SetResult();
}

Task<void> TaskFromDefaultResult<void>::Perform()
{
    TaskCompletionSource<void> tcs;
    tcs.SetResult();
    Task<void> task(std::move(tcs));
    return task;
}

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
    auto whenAllPromise = std::make_shared<detail::WhenAllPromise<void>>();
    whenAllPromise->count = static_cast<int>(tasks.size());
    whenAllPromise->isRejected = false;

    for (auto& task : tasks) {
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

} // namespace pomdog::concurrency::detail
