// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Signals/ConnectionList.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
