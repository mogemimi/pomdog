// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

SystemEventQueue::SystemEventQueue() = default;

void SystemEventQueue::enqueue(const SystemEvent& event)
{
    std::lock_guard<std::mutex> lock{mutex_};
    events_.push_back(event);
}

void SystemEventQueue::enqueue(SystemEvent&& event)
{
    std::lock_guard<std::mutex> lock{mutex_};
    events_.push_back(std::move(event));
}

void SystemEventQueue::emit(const std::function<void(SystemEvent event)>& callback)
{
    POMDOG_ASSERT(callback);
    POMDOG_ASSERT(notifications_.empty());

    {
        std::lock_guard<std::mutex> lock{mutex_};
        std::swap(notifications_, events_);
        POMDOG_ASSERT(events_.empty());
    }

    for (auto& event : notifications_) {
        callback(std::move(event));
    }
    notifications_.clear();
}

void SystemEventQueue::reserve(std::size_t capacity)
{
    {
        std::lock_guard<std::mutex> lock{mutex_};
        events_.reserve(capacity);
    }
    notifications_.reserve(capacity);
}

} // namespace pomdog::detail
