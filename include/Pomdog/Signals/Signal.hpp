// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Signals/detail/SignalBody.hpp"
#include "Pomdog/Signals/Connection.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <functional>
#include <utility>

namespace Pomdog {

template <typename...Arguments>
class POMDOG_EXPORT Signal<void(Arguments...)> {
public:
    Signal();
    Signal(Signal const&) = delete;
    Signal(Signal &&) = default;
    Signal & operator=(Signal const&) = delete;
    Signal & operator=(Signal &&) = default;

    Connection Connect(std::function<void(Arguments...)> const& slot);

    Connection Connect(std::function<void(Arguments...)> && slot);

    void operator()(Arguments... arguments);

    std::size_t InvocationCount() const;

private:
    typedef Detail::Signals::SignalBody<void(Arguments...)> SignalBody;
    std::shared_ptr<SignalBody> body;
};

//-----------------------------------------------------------------------
template <typename...Arguments>
Signal<void(Arguments...)>::Signal()
    : body(std::make_shared<SignalBody>())
{}
//-----------------------------------------------------------------------
template <typename...Arguments>
Connection Signal<void(Arguments...)>::Connect(
    std::function<void(Arguments...)> const& slot)
{
    POMDOG_ASSERT(slot);
    POMDOG_ASSERT(body);
    return Connection{body->Connect(slot)};
}
//-----------------------------------------------------------------------
template <typename...Arguments>
Connection Signal<void(Arguments...)>::Connect(
    std::function<void(Arguments...)> && slot)
{
    POMDOG_ASSERT(slot);
    POMDOG_ASSERT(body);
    return Connection{body->Connect(std::move(slot))};
}
//-----------------------------------------------------------------------
template <typename...Arguments>
void Signal<void(Arguments...)>::operator()(Arguments... arguments)
{
    POMDOG_ASSERT(body);
    body->operator()(std::forward<Arguments>(arguments)...);
}
//-----------------------------------------------------------------------
template <typename...Arguments>
std::size_t Signal<void(Arguments...)>::InvocationCount() const
{
    return body->InvocationCount();
}

} // namespace Pomdog
