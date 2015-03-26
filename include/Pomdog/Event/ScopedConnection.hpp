// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SCOPEDCONNECTION_009320AB_HPP
#define POMDOG_SCOPEDCONNECTION_009320AB_HPP

#include "detail/ForwardDeclarations.hpp"
#include "EventConnection.hpp"
#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class POMDOG_EXPORT ScopedConnection final {
private:
    EventConnection connection;

public:
    ScopedConnection() = default;
    ScopedConnection(ScopedConnection const&) = delete;
    ScopedConnection(ScopedConnection &&) = default;

    ScopedConnection(EventConnection const& connection);
    ScopedConnection(EventConnection && connection);

    ~ScopedConnection();

    ScopedConnection & operator=(ScopedConnection const&) = delete;
    ScopedConnection & operator=(ScopedConnection &&) = default;

    ScopedConnection & operator=(EventConnection const& c);
    ScopedConnection & operator=(EventConnection && c);

    void Disconnect();
};

}// namespace Pomdog

#endif // POMDOG_SCOPEDCONNECTION_009320AB_HPP
