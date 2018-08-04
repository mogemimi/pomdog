// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/Connection.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <memory>
#include <utility>

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
