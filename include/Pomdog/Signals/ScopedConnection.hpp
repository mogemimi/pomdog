// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SCOPEDCONNECTION_009320AB_HPP
#define POMDOG_SCOPEDCONNECTION_009320AB_HPP

#include "detail/ForwardDeclarations.hpp"
#include "Connection.hpp"
#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class POMDOG_EXPORT ScopedConnection final {
private:
    Connection connection;

public:
    ScopedConnection() = default;
    ScopedConnection(ScopedConnection const&) = delete;
    ScopedConnection(ScopedConnection &&) = default;

    ScopedConnection(Connection const& connection);
    ScopedConnection(Connection && connection);

    ~ScopedConnection();

    ScopedConnection & operator=(ScopedConnection const&) = delete;
    ScopedConnection & operator=(ScopedConnection &&) = default;

    ScopedConnection & operator=(Connection const& c);
    ScopedConnection & operator=(Connection && c);

    void Disconnect();
};

} // namespace Pomdog

#endif // POMDOG_SCOPEDCONNECTION_009320AB_HPP
