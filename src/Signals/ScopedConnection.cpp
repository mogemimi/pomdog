// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Signals/ScopedConnection.hpp"
#include <utility>

namespace Pomdog {

ScopedConnection::ScopedConnection(const Connection& c)
    : connection(c)
{
}

ScopedConnection::ScopedConnection(Connection && c)
    : connection(std::move(c))
{
}

ScopedConnection::~ScopedConnection()
{
    connection.Disconnect();
}

ScopedConnection & ScopedConnection::operator=(const Connection& c)
{
    connection.Disconnect();
    connection = c;
    return *this;
}

ScopedConnection & ScopedConnection::operator=(Connection && c)
{
    connection.Disconnect();
    connection = std::move(c);
    return *this;
}

void ScopedConnection::Disconnect()
{
    connection.Disconnect();
}

} // namespace Pomdog
