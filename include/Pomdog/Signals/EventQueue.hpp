// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/ForwardDeclarations.hpp"
#include "Event.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <functional>
#include <utility>
#include <vector>
#include <mutex>

namespace Pomdog {

class POMDOG_EXPORT EventQueue {
public:
    EventQueue();
    EventQueue(EventQueue const&) = delete;
    EventQueue(EventQueue &&) = delete;
    EventQueue & operator=(EventQueue const&) = delete;
    EventQueue & operator=(EventQueue &&) = delete;

    Connection Connect(std::function<void(Event const&)> const& slot);

    Connection Connect(std::function<void(Event const&)> && slot);

    void Enqueue(Event && event);

    template <typename T, typename... Arguments>
    void Enqueue(Arguments &&... arguments)
    {
        Enqueue(Event{T{std::forward<Arguments>(arguments)...}});
    }

    void Emit();

private:
    typedef Detail::Signals::SignalBody<void(Event const&)> SignalBody;
    std::vector<Event> events;
    std::shared_ptr<SignalBody> signalBody;
    std::recursive_mutex notificationProtection;
};

} // namespace Pomdog
