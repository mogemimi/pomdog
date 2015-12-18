// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_CONNECTION_8E4EC7A9_HPP
#define POMDOG_CONNECTION_8E4EC7A9_HPP

#include "detail/ForwardDeclarations.hpp"
#include "detail/SignalBody.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <utility>

namespace Pomdog {

class POMDOG_EXPORT Connection {
private:
    typedef Detail::Signals::ConnectionBody ConnectionBody;
    std::unique_ptr<ConnectionBody> body;

public:
    Connection() = default;

    explicit Connection(std::unique_ptr<ConnectionBody> && body);

    Connection(Connection const& connection);
    Connection(Connection && connection) = default;

    Connection & operator=(Connection const& connection);
    Connection & operator=(Connection && connection) = default;

    operator bool() const noexcept;

    void Disconnect();
};

} // namespace Pomdog

#endif // POMDOG_CONNECTION_8E4EC7A9_HPP
