// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Event/Connection.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
Connection::Connection(std::unique_ptr<ConnectionBody> && bodyIn)
    : body(std::forward<std::unique_ptr<ConnectionBody>>(bodyIn))
{}
//-----------------------------------------------------------------------
Connection::Connection(Connection const& connection)
{
    if (connection.body) {
        body = connection.body->DeepCopy();
    }
}
//-----------------------------------------------------------------------
Connection & Connection::operator=(Connection const& connection)
{
    if (connection.body) {
        body = connection.body->DeepCopy();
    }
    return *this;
}
//-----------------------------------------------------------------------
void Connection::Disconnect()
{
    if (body) {
        body->Disconnect();
    }
}
//-----------------------------------------------------------------------
} // namespace Pomdog
