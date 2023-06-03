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
private:
    using SignalBody = detail::signals::SignalBody<void(const T&)>;
    std::vector<T> events_;
    std::vector<T> notifications_;
    std::shared_ptr<SignalBody> signalBody_;
    detail::SpinLock notificationProtection_;

public:
    EventQueue();
    EventQueue(const EventQueue&) = delete;
    EventQueue(EventQueue&&) = delete;
    EventQueue& operator=(const EventQueue&) = delete;
    EventQueue& operator=(EventQueue&&) = delete;

    [[nodiscard]] Connection
    connect(const std::function<void(const T&)>& slot);

    [[nodiscard]] Connection
    connect(std::function<void(const T&)>&& slot);

    void enqueue(const T& event);

    void enqueue(T&& event);

    void emit();

    void reserve(std::size_t capacity);
};

template <typename T>
EventQueue<T>::EventQueue()
    : signalBody_(std::make_shared<SignalBody>())
{
}

template <typename T>
Connection
EventQueue<T>::connect(const std::function<void(const T&)>& slot)
{
    POMDOG_ASSERT(slot);
    POMDOG_ASSERT(signalBody_);
    return Connection{signalBody_->connect(slot)};
}

template <typename T>
Connection
EventQueue<T>::connect(std::function<void(const T&)>&& slot)
{
    POMDOG_ASSERT(slot);
    POMDOG_ASSERT(signalBody_);
    return Connection{signalBody_->connect(slot)};
}

template <typename T>
void EventQueue<T>::enqueue(const T& event)
{
    std::lock_guard<detail::SpinLock> lock{notificationProtection_};
    events_.push_back(event);
}

template <typename T>
void EventQueue<T>::enqueue(T&& event)
{
    std::lock_guard<detail::SpinLock> lock{notificationProtection_};
    events_.push_back(std::move(event));
}

template <typename T>
void EventQueue<T>::emit()
{
    POMDOG_ASSERT(signalBody_);
    POMDOG_ASSERT(notifications_.empty());

    {
        std::lock_guard<detail::SpinLock> lock{notificationProtection_};
        std::swap(notifications_, events_);
        POMDOG_ASSERT(events_.empty());
    }

    for (auto& event : notifications_) {
        signalBody_->emit(event);
    }
    notifications_.clear();
}

template <typename T>
void EventQueue<T>::reserve(std::size_t capacity)
{
    {
        std::lock_guard<detail::SpinLock> lock{notificationProtection_};
        events_.reserve(capacity);
    }
    notifications_.reserve(capacity);
}

} // namespace pomdog
