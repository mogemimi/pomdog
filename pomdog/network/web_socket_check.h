// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <expected>
#include <memory>
#include <span>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// Checks the subprotocols that a client offers. Fails when one is not a
/// token (RFC 9110, section 5.6.2) or two are the same, as browsers do.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
checkWebSocketProtocols(std::span<const std::string> protocols);

/// Checks the arguments of `WebSocket::close()`. Fails, as browsers do, when
/// code is neither 1000 nor from 3000 to 4999, and when reason is not valid
/// UTF-8 or longer than 123 bytes.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
checkWebSocketClose(u16 code, std::string_view reason);

/// Returns true if text is valid UTF-8, without overlong forms, surrogates,
/// or code points above U+10FFFF, false otherwise.
[[nodiscard]] bool
isValidUTF8(std::string_view text) noexcept;

} // namespace pomdog::network::detail
