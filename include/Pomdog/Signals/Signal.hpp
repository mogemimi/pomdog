// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/Connection.hpp"
#include "Pomdog/Signals/detail/SignalBody.hpp"
#include <functional>
#include <memory>
#include <utility>

namespace Pomdog {

template <typename... Arguments>
class POMDOG_EXPORT Signal<void(Arguments...)> final {
public:
    Signal();
    Signal(const Signal&) = delete;
    Signal(Signal&&) = default;
    Signal& operator=(const Signal&) = delete;
    Signal& operator=(Signal&&) = default;

    Connection Connect(const std::function<void(Arguments...)>& slot);

    Connection Connect(std::function<void(Arguments...)>&& slot);

    void operator()(Arguments... arguments);

    [[nodiscard]] std::size_t GetInvocationCount() const;

private:
    using SignalBody = Detail::Signals::SignalBody<void(Arguments...)>;
    std::shared_ptr<SignalBody> body;
};

template <typename... Arguments>
Signal<void(Arguments...)>::Signal()
    : body(std::make_shared<SignalBody>())
{
}

template <typename... Arguments>
Connection Signal<void(Arguments...)>::Connect(
    const std::function<void(Arguments...)>& slot)
{
    POMDOG_ASSERT(slot);
    POMDOG_ASSERT(body);
    Connection conn{body->Connect(slot)};
    return conn;
}

template <typename... Arguments>
Connection Signal<void(Arguments...)>::Connect(
    std::function<void(Arguments...)>&& slot)
{
    POMDOG_ASSERT(slot);
    POMDOG_ASSERT(body);
    Connection conn{body->Connect(std::move(slot))};
    return conn;
}

template <typename... Arguments>
void Signal<void(Arguments...)>::operator()(Arguments... arguments)
{
    POMDOG_ASSERT(body);
    body->Emit(std::forward<Arguments>(arguments)...);
}

template <typename... Arguments>
std::size_t Signal<void(Arguments...)>::GetInvocationCount() const
{
    return body->GetInvocationCount();
}

} // namespace Pomdog
