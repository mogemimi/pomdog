// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Signals/Connection.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <utility>
#include <memory>

namespace Pomdog {
namespace Signals {

template <typename...Arguments, typename Func> POMDOG_EXPORT
Connection ConnectSingleShot(Signal<void(Arguments...)> & signal, Func && func)
{
    auto connection = std::make_shared<Connection>();
    *connection = signal.Connect([
        conn = connection,
        func = std::forward<Func>(func)
    ](Arguments... args) {
        func(std::forward<Arguments>(args)...);
        POMDOG_ASSERT(conn);
        conn->Disconnect();
    });
    return *connection;
}

} // namespace Signals
} // namespace Pomdog
