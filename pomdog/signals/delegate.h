// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/signals/connection.h"
#include "pomdog/signals/detail/delegate_body.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

template <typename... Arguments>
class POMDOG_EXPORT Delegate<void(Arguments...)> final {
private:
    using Body = detail::signals::DelegateBody<void(Arguments...)>;
    std::shared_ptr<Body> body_;

public:
    Delegate();
    Delegate(const Delegate&) = delete;
    Delegate(Delegate&&) = default;
    Delegate& operator=(const Delegate&) = delete;
    Delegate& operator=(Delegate&&) = default;

    [[nodiscard]] Connection
    connect(const std::function<void(Arguments...)>& slot);

    [[nodiscard]] Connection
    connect(std::function<void(Arguments...)>&& slot);

    void disconnect();

    void operator()(Arguments... arguments);

    [[nodiscard]] bool
    isConnected() const noexcept;
};

template <typename... Arguments>
Delegate<void(Arguments...)>::Delegate()
    : body_(std::make_shared<Body>())
{
}

template <typename... Arguments>
void Delegate<void(Arguments...)>::operator()(Arguments... arguments)
{
    POMDOG_ASSERT(body_ != nullptr);
    body_->emit(std::forward<Arguments>(arguments)...);
}

template <typename... Arguments>
Connection Delegate<void(Arguments...)>::connect(const std::function<void(Arguments...)>& slot)
{
    POMDOG_ASSERT(body_ != nullptr);
    return Connection{body_->Connect(slot)};
}

template <typename... Arguments>
Connection Delegate<void(Arguments...)>::connect(std::function<void(Arguments...)>&& slot)
{
    POMDOG_ASSERT(body_ != nullptr);
    return Connection{body_->connect(std::move(slot))};
}

template <typename... Arguments>
void Delegate<void(Arguments...)>::disconnect()
{
    POMDOG_ASSERT(body_ != nullptr);
    body_->disconnect();
}

template <typename... Arguments>
bool Delegate<void(Arguments...)>::isConnected() const noexcept
{
    POMDOG_ASSERT(body_ != nullptr);
    return body_->isConnected();
}

} // namespace pomdog
