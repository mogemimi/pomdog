// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CONNECTIONLIST_EF8910DE_HPP
#define POMDOG_CONNECTIONLIST_EF8910DE_HPP

#include "Pomdog/Signals/Connection.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <vector>
#include <utility>

namespace Pomdog {

class POMDOG_EXPORT ConnectionList {
private:
    std::vector<Connection> connections;

public:
    ConnectionList() = default;
    ConnectionList(ConnectionList const&) = delete;
    ConnectionList & operator=(ConnectionList const&) = delete;
    ConnectionList(ConnectionList &&) = default;
    ConnectionList & operator=(ConnectionList &&) = default;

    ~ConnectionList();

    void operator+=(Connection && connection);

    template <typename...Args, typename Func>
    Connection operator()(Signal<void(Args...)> & signal, Func && func)
    {
        auto connection = signal.Connect(std::forward<Func>(func));
        connections.push_back(connection);
        return std::move(connection);
    }

    void Disconnect();
};

} // namespace Pomdog

#endif // POMDOG_CONNECTIONLIST_EF8910DE_HPP
