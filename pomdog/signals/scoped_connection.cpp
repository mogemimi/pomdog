// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/signals/scoped_connection.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

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

} // namespace pomdog
