// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Event/ScopedConnection.hpp"
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
ScopedConnection::ScopedConnection(Connection const& c)
    : connection(c)
{}
//-----------------------------------------------------------------------
ScopedConnection::ScopedConnection(Connection && c)
    : connection(std::move(c))
{}
//-----------------------------------------------------------------------
ScopedConnection::~ScopedConnection()
{
    connection.Disconnect();
}
//-----------------------------------------------------------------------
ScopedConnection & ScopedConnection::operator=(Connection const& c)
{
    connection.Disconnect();
    connection = c;
    return *this;
}
//-----------------------------------------------------------------------
ScopedConnection & ScopedConnection::operator=(Connection && c)
{
    connection.Disconnect();
    connection = std::move(c);
    return *this;
}
//-----------------------------------------------------------------------
void ScopedConnection::Disconnect()
{
    connection.Disconnect();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
