// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/signals/connection.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT ConnectionList final {
private:
    std::vector<Connection> connections;

public:
    ConnectionList() = default;
    ConnectionList(const ConnectionList&) = delete;
    ConnectionList& operator=(const ConnectionList&) = delete;
    ConnectionList(ConnectionList&&) = default;
    ConnectionList& operator=(ConnectionList&&) = default;

    ~ConnectionList();

    void operator+=(Connection&& connection);

    template <typename... Args, typename Func>
    Connection operator()(Signal<void(Args...)>& signal, Func&& func)
    {
        auto connection = signal.Connect(std::forward<Func>(func));
        connections.push_back(connection);
        return connection;
    }

    void Disconnect();
};

} // namespace pomdog
