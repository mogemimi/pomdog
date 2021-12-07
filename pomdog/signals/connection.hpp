// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/signals/detail/signal_body.hpp"
#include "pomdog/signals/forward_declarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class POMDOG_EXPORT Connection final {
private:
    using ConnectionBody = Detail::Signals::ConnectionBody;
    std::unique_ptr<ConnectionBody> body;

public:
    Connection() = default;

    explicit Connection(std::unique_ptr<ConnectionBody>&& body);

    Connection(const Connection& connection);
    Connection(Connection&& connection) = default;

    Connection& operator=(const Connection& connection);
    Connection& operator=(Connection&& connection) = default;

    void Disconnect();

    [[nodiscard]] bool IsConnected() const;
};

} // namespace Pomdog
