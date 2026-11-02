// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <expected>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// WinsockLibrary keeps Winsock started while it lives.
///
/// Winsock counts WSAStartup() calls and stops at the matching last
/// WSACleanup(), so each object that owns sockets holds its own instance.
/// Declare it before the sockets, so that it cleans up after they close.
class WinsockLibrary final {
public:
    WinsockLibrary() noexcept;

    WinsockLibrary(const WinsockLibrary&) = delete;
    WinsockLibrary& operator=(const WinsockLibrary&) = delete;

    ~WinsockLibrary();

    /// Starts Winsock 2.2. Call it once, before using sockets.
    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    startup();

private:
    bool started_ = false;
};

} // namespace pomdog::network::detail
