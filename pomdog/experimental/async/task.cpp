// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/async/task.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <atomic>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::concurrency::detail {

void TaskImpl::innerSetResult(
    const TaskCompletionSource<void>& tcs,
    const TaskResult<void>&)
{
    tcs.setResult();
}

Task<void> TaskFromDefaultResult<void>::perform()
{
    TaskCompletionSource<void> tcs;
    tcs.setResult();
    Task<void> task(std::move(tcs));
    return task;
}

Task<void> whenAllImpl(const std::vector<Task<void>>& tasks)
{
    if (tasks.empty()) {
        TaskCompletionSource<void> tcs;
        tcs.setResult();
        Task<void> task(std::move(tcs));
        return task;
    }

    POMDOG_ASSERT(!tasks.empty());

    TaskCompletionSource<void> tcs;
    auto whenAllPromise = std::make_shared<detail::WhenAllPromise<void>>();
    whenAllPromise->count = static_cast<int>(tasks.size());
    whenAllPromise->isRejected = false;

    for (auto& task : tasks) {
        task.continueWith([tcs, whenAllPromise](const Task<void>& t) {
            std::lock_guard<std::mutex> lock(whenAllPromise->mutex);
            if (whenAllPromise->isRejected) {
                return;
            }
            if (t.isRejected()) {
                whenAllPromise->isRejected = true;
                tcs.setException(TaskImpl::getExceptionPointer(t));
            }
            else {
                --whenAllPromise->count;
                POMDOG_ASSERT(whenAllPromise->count >= 0);
                if (whenAllPromise->count <= 0) {
                    tcs.setResult();
                }
            }
        });
    }
    Task<void> task(std::move(tcs));
    return task;
}

} // namespace pomdog::concurrency::detail
