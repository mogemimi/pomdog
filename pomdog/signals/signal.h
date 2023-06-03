// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/signals/connection.h"
#include "pomdog/signals/detail/signal_body.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

template <typename... Arguments>
class POMDOG_EXPORT Signal<void(Arguments...)> final {
private:
    using SignalBody = detail::signals::SignalBody<void(Arguments...)>;
    std::shared_ptr<SignalBody> body_;

public:
    Signal();
    Signal(const Signal&) = delete;
    Signal(Signal&&) = default;
    Signal& operator=(const Signal&) = delete;
    Signal& operator=(Signal&&) = default;

    [[nodiscard]] Connection
    connect(const std::function<void(Arguments...)>& slot);

    [[nodiscard]] Connection
    connect(std::function<void(Arguments...)>&& slot);

    void operator()(Arguments... arguments);

    [[nodiscard]] std::size_t
    getInvocationCount() const;
};

template <typename... Arguments>
Signal<void(Arguments...)>::Signal()
    : body_(std::make_shared<SignalBody>())
{
}

template <typename... Arguments>
Connection
Signal<void(Arguments...)>::connect(const std::function<void(Arguments...)>& slot)
{
    POMDOG_ASSERT(slot);
    POMDOG_ASSERT(body_);
    Connection conn{body_->connect(slot)};
    return conn;
}

template <typename... Arguments>
Connection
Signal<void(Arguments...)>::connect(std::function<void(Arguments...)>&& slot)
{
    POMDOG_ASSERT(slot);
    POMDOG_ASSERT(body_);
    Connection conn{body_->connect(std::move(slot))};
    return conn;
}

template <typename... Arguments>
void Signal<void(Arguments...)>::operator()(Arguments... arguments)
{
    POMDOG_ASSERT(body_);
    body_->emit(std::forward<Arguments>(arguments)...);
}

template <typename... Arguments>
std::size_t
Signal<void(Arguments...)>::getInvocationCount() const
{
    return body_->getInvocationCount();
}

} // namespace pomdog
