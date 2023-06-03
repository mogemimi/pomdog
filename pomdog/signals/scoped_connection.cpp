// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/signals/scoped_connection.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

ScopedConnection::ScopedConnection(const Connection& c)
    : connection_(c)
{
}

ScopedConnection::ScopedConnection(Connection&& c)
    : connection_(std::move(c))
{
}

ScopedConnection::~ScopedConnection()
{
    connection_.disconnect();
}

ScopedConnection& ScopedConnection::operator=(const Connection& c)
{
    connection_.disconnect();
    connection_ = c;
    return *this;
}

ScopedConnection& ScopedConnection::operator=(Connection&& c)
{
    connection_.disconnect();
    connection_ = std::move(c);
    return *this;
}

void ScopedConnection::disconnect()
{
    connection_.disconnect();
}

bool ScopedConnection::isConnected() const
{
    return connection_.isConnected();
}

} // namespace pomdog
