// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "Pomdog/Signals/Connection.hpp"

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
Connection::operator bool() const noexcept
{
    return body && body->Valid();
}
//-----------------------------------------------------------------------
void Connection::Disconnect()
{
    if (body) {
        body->Disconnect();
        body.reset();
    }
}
//-----------------------------------------------------------------------
} // namespace Pomdog
