// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/signals/connection.hpp"
#include "pomdog/signals/signal.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Signals {

template <typename... Arguments, typename Func>
POMDOG_EXPORT Connection
ConnectSingleShot(Signal<void(Arguments...)>& signal, Func&& func)
{
    auto connection = std::make_shared<Connection>();
    *connection = signal.Connect([conn = connection, func = std::forward<Func>(func)](Arguments... args) {
        func(std::forward<Arguments>(args)...);
        POMDOG_ASSERT(conn);
        conn->Disconnect();
    });
    return *connection;
}

} // namespace Pomdog::Signals
