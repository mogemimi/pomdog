// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Signals/EventQueue.hpp"
#include "Pomdog/Signals/Connection.hpp"
#include "Pomdog/Signals/detail/SignalBody.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <mutex>

namespace Pomdog {

EventQueue::EventQueue()
    : signalBody(std::make_shared<SignalBody>())
{
}

Connection EventQueue::Connect(const std::function<void(const Event&)>& slot)
{
    POMDOG_ASSERT(slot);
    POMDOG_ASSERT(this->signalBody);
    return Connection{signalBody->Connect(slot)};
}

Connection EventQueue::Connect(std::function<void(const Event&)>&& slot)
{
    POMDOG_ASSERT(slot);
    POMDOG_ASSERT(this->signalBody);
    return Connection{signalBody->Connect(slot)};
}

void EventQueue::Enqueue(Event&& event)
{
    std::lock_guard<Detail::SpinLock> lock{notificationProtection};
    events.emplace_back(std::move(event));
}

void EventQueue::Emit()
{
    POMDOG_ASSERT(this->signalBody);

    std::vector<Event> notifications;
    {
        std::lock_guard<Detail::SpinLock> lock{notificationProtection};
        std::swap(notifications, events);
    }

    for (auto& event : notifications) {
        signalBody->Emit(event);
    }
}

} // namespace Pomdog
