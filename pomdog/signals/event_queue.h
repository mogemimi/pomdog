// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/signals/connection.h"
#include "pomdog/signals/detail/signal_body.h"
#include "pomdog/signals/forward_declarations.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/spin_lock.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

template <typename T>
class POMDOG_EXPORT EventQueue final {
public:
    EventQueue();
    EventQueue(const EventQueue&) = delete;
    EventQueue(EventQueue&&) = delete;
    EventQueue& operator=(const EventQueue&) = delete;
    EventQueue& operator=(EventQueue&&) = delete;

    [[nodiscard]] Connection Connect(const std::function<void(const T&)>& slot);

    [[nodiscard]] Connection Connect(std::function<void(const T&)>&& slot);

    void Enqueue(const T& event);

    void Enqueue(T&& event);

    void Emit();

    void Reserve(std::size_t capacity);

private:
    using SignalBody = detail::signals::SignalBody<void(const T&)>;
    std::vector<T> events;
    std::vector<T> notifications;
    std::shared_ptr<SignalBody> signalBody;
    detail::SpinLock notificationProtection;
};

template <typename T>
EventQueue<T>::EventQueue()
    : signalBody(std::make_shared<SignalBody>())
{
}

template <typename T>
Connection
EventQueue<T>::Connect(const std::function<void(const T&)>& slot)
{
    POMDOG_ASSERT(slot);
    POMDOG_ASSERT(this->signalBody);
    return Connection{signalBody->Connect(slot)};
}

template <typename T>
Connection
EventQueue<T>::Connect(std::function<void(const T&)>&& slot)
{
    POMDOG_ASSERT(slot);
    POMDOG_ASSERT(this->signalBody);
    return Connection{signalBody->Connect(slot)};
}

template <typename T>
void EventQueue<T>::Enqueue(const T& event)
{
    std::lock_guard<detail::SpinLock> lock{notificationProtection};
    events.push_back(event);
}

template <typename T>
void EventQueue<T>::Enqueue(T&& event)
{
    std::lock_guard<detail::SpinLock> lock{notificationProtection};
    events.push_back(std::move(event));
}

template <typename T>
void EventQueue<T>::Emit()
{
    POMDOG_ASSERT(this->signalBody);
    POMDOG_ASSERT(notifications.empty());

    {
        std::lock_guard<detail::SpinLock> lock{notificationProtection};
        std::swap(notifications, events);
        POMDOG_ASSERT(events.empty());
    }

    for (auto& event : notifications) {
        signalBody->Emit(event);
    }
    notifications.clear();
}

template <typename T>
void EventQueue<T>::Reserve(std::size_t capacity)
{
    {
        std::lock_guard<detail::SpinLock> lock{notificationProtection};
        events.reserve(capacity);
    }
    notifications.reserve(capacity);
}

} // namespace pomdog
