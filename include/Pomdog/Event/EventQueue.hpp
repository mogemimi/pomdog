// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EVENTQUEUE_4A197647_AE4E_42C7_AC78_B5E621DD2DA2_HPP
#define POMDOG_EVENTQUEUE_4A197647_AE4E_42C7_AC78_B5E621DD2DA2_HPP

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

    EventConnection Connect(std::function<void(Event const&)> const& slot);

    EventConnection Connect(std::function<void(Event const&)> && slot);

    void Enqueue(Event && event);

    template <typename T, typename...Arguments>
    void Enqueue(Arguments && ...arguments)
    {
        Enqueue(Event{T{std::forward<Arguments>(arguments)...}});
    }

    void Tick();

private:
    typedef Detail::SignalsAndSlots::SignalBody<void(Event const&)> SignalType;

    std::vector<Event> events;
    std::shared_ptr<SignalType> signalBody;
    std::recursive_mutex notificationProtection;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EVENTQUEUE_4A197647_AE4E_42C7_AC78_B5E621DD2DA2_HPP)
