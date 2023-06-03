// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/signals/connection_list.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

ConnectionList::~ConnectionList()
{
    for (auto& connection : connections_) {
        connection.disconnect();
    }
}

void ConnectionList::disconnect()
{
    for (auto& connection : connections_) {
        connection.disconnect();
    }
    connections_.clear();
}

void ConnectionList::operator+=(Connection&& connection)
{
    connections_.push_back(std::move(connection));
}

} // namespace pomdog
