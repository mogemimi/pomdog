// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/Connection.hpp"
#include "Pomdog/Signals/detail/ForwardDeclarations.hpp"

namespace Pomdog {

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
};

} // namespace Pomdog
