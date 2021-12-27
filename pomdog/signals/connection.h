// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/signals/detail/signal_body.h"
#include "pomdog/signals/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT Connection final {
private:
    using ConnectionBody = detail::signals::ConnectionBody;
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

} // namespace pomdog
