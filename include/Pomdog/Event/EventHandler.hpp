// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EVENTHANDLER_13D11737_HPP
#define POMDOG_EVENTHANDLER_13D11737_HPP

#include "detail/ForwardDeclarations.hpp"
#include "Event.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <functional>
#include <utility>

namespace Pomdog {

class POMDOG_EXPORT EventHandler {
public:
    EventHandler();
    EventHandler(EventHandler const&) = delete;
    EventHandler(EventHandler &&) = default;
    EventHandler & operator=(EventHandler const&) = delete;
    EventHandler & operator=(EventHandler &&) = default;

    Connection Connect(std::function<void(Event const&)> const& slot);

    Connection Connect(std::function<void(Event const&)> && slot);

    void Invoke(Event && event);

    template <typename T, typename...Arguments>
    void Invoke(Arguments && ...arguments)
    {
        Invoke(Event{T{std::forward<Arguments>(arguments)...}});
    }

private:
    typedef Detail::Signals::SignalBody<void(Event const&)> SignalBody;
    std::shared_ptr<SignalBody> signalBody;
};

} // namespace Pomdog

#endif // POMDOG_EVENTHANDLER_13D11737_HPP
