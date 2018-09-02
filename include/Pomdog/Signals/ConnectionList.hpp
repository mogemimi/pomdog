// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/Connection.hpp"
#include <utility>
#include <vector>

namespace Pomdog {

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
        return std::move(connection);
    }

    void Disconnect();
};

} // namespace Pomdog
