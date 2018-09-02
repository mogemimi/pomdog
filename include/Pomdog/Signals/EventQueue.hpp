// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/Event.hpp"
#include "Pomdog/Signals/detail/ForwardDeclarations.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace Pomdog {

class POMDOG_EXPORT EventQueue final {
public:
    EventQueue();
    EventQueue(const EventQueue&) = delete;
    EventQueue(EventQueue&&) = delete;
    EventQueue& operator=(const EventQueue&) = delete;
    EventQueue& operator=(EventQueue&&) = delete;

    Connection Connect(const std::function<void(const Event&)>& slot);

    Connection Connect(std::function<void(const Event&)>&& slot);

    void Enqueue(Event&& event);

    template <typename T, typename... Arguments>
    void Enqueue(Arguments&&... arguments)
    {
        Enqueue(Event{T{std::forward<Arguments>(arguments)...}});
    }

    void Emit();

private:
    using SignalBody = Detail::Signals::SignalBody<void(const Event&)>;
    std::vector<Event> events;
    std::shared_ptr<SignalBody> signalBody;
    std::recursive_mutex notificationProtection;
};

} // namespace Pomdog
