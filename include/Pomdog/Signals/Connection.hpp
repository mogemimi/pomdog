// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Signals/detail/ForwardDeclarations.hpp"
#include "Pomdog/Signals/detail/SignalBody.hpp"
#include <memory>
#include <utility>

namespace Pomdog {

class POMDOG_EXPORT Connection final {
private:
    typedef Detail::Signals::ConnectionBody ConnectionBody;
    std::unique_ptr<ConnectionBody> body;

public:
    Connection() = default;

    explicit Connection(std::unique_ptr<ConnectionBody>&& body);

    Connection(const Connection& connection);
    Connection(Connection&& connection) = default;

    Connection& operator=(const Connection& connection);
    Connection& operator=(Connection&& connection) = default;

    operator bool() const noexcept;

    void Disconnect();
};

} // namespace Pomdog
