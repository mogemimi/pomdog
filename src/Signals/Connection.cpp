// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Signals/Connection.hpp"

namespace Pomdog {

Connection::Connection(std::unique_ptr<ConnectionBody> && bodyIn)
    : body(std::forward<std::unique_ptr<ConnectionBody>>(bodyIn))
{
}

Connection::Connection(const Connection& connection)
{
    if (connection.body) {
        body = connection.body->DeepCopy();
    }
}

Connection & Connection::operator=(const Connection& connection)
{
    if (connection.body) {
        body = connection.body->DeepCopy();
    }
    return *this;
}

void Connection::Disconnect()
{
    if (body) {
        body->Disconnect();
        body.reset();
    }
}

bool Connection::IsConnected() const noexcept
{
    return body && body->Valid();
}

} // namespace Pomdog
