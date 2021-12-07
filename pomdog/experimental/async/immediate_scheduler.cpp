// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/async/immediate_scheduler.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <thread>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Concurrency {

void ImmediateScheduler::Schedule(
    std::function<void()>&& task,
    const Duration& delayTime)
{
    POMDOG_ASSERT(task);
    if (delayTime > std::chrono::duration<double>::zero()) {
        using namespace std::chrono;
        std::this_thread::sleep_for(duration_cast<milliseconds>(delayTime));
    }
    task();
}

} // namespace Pomdog::Concurrency
