// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Signals/Connection.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

Connection::Connection(std::unique_ptr<ConnectionBody>&& bodyIn)
    : body(std::move(bodyIn))
{
}

Connection::Connection(const Connection& connection)
{
    if (connection.body != nullptr) {
        body = connection.body->DeepCopy();
    }
}

Connection& Connection::operator=(const Connection& connection)
{
    if (connection.body != nullptr) {
        body = connection.body->DeepCopy();
    }
    return *this;
}

void Connection::Disconnect()
{
    if (body != nullptr) {
        body->Disconnect();
        body.reset();
    }
}

bool Connection::IsConnected() const
{
    return (body != nullptr) && body->Valid();
}

} // namespace Pomdog
