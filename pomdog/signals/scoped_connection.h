// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/signals/connection.h"
#include "pomdog/signals/forward_declarations.h"

namespace pomdog {

class POMDOG_EXPORT ScopedConnection final {
private:
    Connection connection;

public:
    ScopedConnection() = default;
    ScopedConnection(const ScopedConnection&) = delete;
    ScopedConnection(ScopedConnection&&) = default;

    ScopedConnection(const Connection& connection);
    ScopedConnection(Connection&& connection);

    ~ScopedConnection();

    ScopedConnection& operator=(const ScopedConnection&) = delete;
    ScopedConnection& operator=(ScopedConnection&&) = default;

    ScopedConnection& operator=(const Connection& c);
    ScopedConnection& operator=(Connection&& c);

    void Disconnect();

    bool IsConnected() const;
};

} // namespace pomdog
