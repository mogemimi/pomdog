// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/Connection.hpp"
#include "Pomdog/Signals/Detail/SignalBody.hpp"
#include "Pomdog/Signals/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Detail/SpinLock.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

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
    using SignalBody = Detail::Signals::SignalBody<void(const T&)>;
    std::vector<T> events;
    std::vector<T> notifications;
    std::shared_ptr<SignalBody> signalBody;
    Detail::SpinLock notificationProtection;
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
    std::lock_guard<Detail::SpinLock> lock{notificationProtection};
    events.push_back(event);
}

template <typename T>
void EventQueue<T>::Enqueue(T&& event)
{
    std::lock_guard<Detail::SpinLock> lock{notificationProtection};
    events.push_back(std::move(event));
}

template <typename T>
void EventQueue<T>::Emit()
{
    POMDOG_ASSERT(this->signalBody);
    POMDOG_ASSERT(notifications.empty());

    {
        std::lock_guard<Detail::SpinLock> lock{notificationProtection};
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
        std::lock_guard<Detail::SpinLock> lock{notificationProtection};
        events.reserve(capacity);
    }
    notifications.reserve(capacity);
}

} // namespace Pomdog
