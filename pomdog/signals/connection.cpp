// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/signals/connection.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Connection::Connection(std::unique_ptr<ConnectionBody>&& bodyIn)
    : body_(std::move(bodyIn))
{
}

Connection::Connection(const Connection& connection)
{
    if (connection.body_ != nullptr) {
        body_ = connection.body_->deepCopy();
    }
}

Connection& Connection::operator=(const Connection& connection)
{
    if (connection.body_ != nullptr) {
        body_ = connection.body_->deepCopy();
    }
    return *this;
}

void Connection::disconnect()
{
    if (body_ != nullptr) {
        body_->disconnect();
        body_.reset();
    }
}

bool Connection::isConnected() const
{
    return (body_ != nullptr) && body_->valid();
}

} // namespace pomdog
