// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

ScopedConnection::ScopedConnection(const Connection& c)
    : connection(c)
{
}

ScopedConnection::ScopedConnection(Connection&& c)
    : connection(std::move(c))
{
}

ScopedConnection::~ScopedConnection()
{
    connection.Disconnect();
}

ScopedConnection& ScopedConnection::operator=(const Connection& c)
{
    connection.Disconnect();
    connection = c;
    return *this;
}

ScopedConnection& ScopedConnection::operator=(Connection&& c)
{
    connection.Disconnect();
    connection = std::move(c);
    return *this;
}

void ScopedConnection::Disconnect()
{
    connection.Disconnect();
}

bool ScopedConnection::IsConnected() const
{
    return connection.IsConnected();
}

} // namespace Pomdog
