// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/network/stream.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <expected>
#include <memory>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network {

/// Starts a TLS connection over TCP to address, such as "example.com:443".
///
/// The stream checks the server's certificate against the root certificates
/// built into Pomdog, and fails when it does not verify in Debug builds only.
/// It reports Connected after the handshake, or Disconnected with an error,
/// after a later `Stream::update()`. Returns an error when the connection
/// cannot start, and on the web, which has no TCP sockets.
[[nodiscard]] POMDOG_EXPORT std::expected<std::shared_ptr<Stream>, std::unique_ptr<Error>>
connectTLSStream(std::string_view address, const StreamOptions& options);

} // namespace pomdog::network
