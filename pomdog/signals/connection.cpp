// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/signals/connection.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

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

} // namespace pomdog
