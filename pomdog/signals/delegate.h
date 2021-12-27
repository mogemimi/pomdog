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
public:
    Delegate();
    Delegate(const Delegate&) = delete;
    Delegate(Delegate&&) = default;
    Delegate& operator=(const Delegate&) = delete;
    Delegate& operator=(Delegate&&) = default;

    [[nodiscard]] Connection Connect(const std::function<void(Arguments...)>& slot);

    [[nodiscard]] Connection Connect(std::function<void(Arguments...)>&& slot);

    void Disconnect();

    void operator()(Arguments... arguments);

    [[nodiscard]] operator bool() const noexcept;

    [[nodiscard]] bool IsConnected() const noexcept;

private:
    using Body = detail::signals::DelegateBody<void(Arguments...)>;
    std::shared_ptr<Body> body;
};

template <typename... Arguments>
Delegate<void(Arguments...)>::Delegate()
    : body(std::make_shared<Body>())
{
}

template <typename... Arguments>
void Delegate<void(Arguments...)>::operator()(Arguments... arguments)
{
    POMDOG_ASSERT(body != nullptr);
    body->Emit(std::forward<Arguments>(arguments)...);
}

template <typename... Arguments>
Connection Delegate<void(Arguments...)>::Connect(const std::function<void(Arguments...)>& slot)
{
    POMDOG_ASSERT(body != nullptr);
    return Connection{body->Connect(slot)};
}

template <typename... Arguments>
Connection Delegate<void(Arguments...)>::Connect(std::function<void(Arguments...)>&& slot)
{
    POMDOG_ASSERT(body != nullptr);
    return Connection{body->Connect(std::move(slot))};
}

template <typename... Arguments>
void Delegate<void(Arguments...)>::Disconnect()
{
    POMDOG_ASSERT(body != nullptr);
    body->Disconnect();
}

template <typename... Arguments>
Delegate<void(Arguments...)>::operator bool() const noexcept
{
    POMDOG_ASSERT(body != nullptr);
    return body->IsConnected();
}

template <typename... Arguments>
bool Delegate<void(Arguments...)>::IsConnected() const noexcept
{
    POMDOG_ASSERT(body != nullptr);
    return body->IsConnected();
}

} // namespace pomdog
