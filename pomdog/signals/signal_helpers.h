// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/signals/connection.h"
#include "pomdog/signals/signal.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::signals {

template <typename... Arguments, typename Func>
POMDOG_EXPORT Connection
connectSingleShot(Signal<void(Arguments...)>& signal, Func&& func)
{
    auto connection = std::make_shared<Connection>();
    *connection = signal.connect([conn = connection, func = std::forward<Func>(func)](Arguments... args) {
        func(std::forward<Arguments>(args)...);
        POMDOG_ASSERT(conn);
        conn->disconnect();
    });
    return *connection;
}

} // namespace pomdog::signals
