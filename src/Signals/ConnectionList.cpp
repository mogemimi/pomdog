// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Signals/ConnectionList.hpp"
#include <utility>

namespace Pomdog {

ConnectionList::~ConnectionList()
{
    for (auto& connection : connections) {
        connection.Disconnect();
    }
}

void ConnectionList::Disconnect()
{
    for (auto& connection : connections) {
        connection.Disconnect();
    }
    connections.clear();
}

void ConnectionList::operator+=(Connection&& connection)
{
    connections.push_back(std::move(connection));
}

} // namespace Pomdog
