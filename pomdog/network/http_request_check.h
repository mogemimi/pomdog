// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <expected>
#include <memory>
#include <span>
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// Checks the headers of an `HTTPRequest` before the client sends it.
///
/// Fails when a name is not an HTTP token, when a value has a line break or a
/// null character, and for the headers that the client sets itself: Host
/// (from the URL), Connection (from `persistentConnection`), Content-Length
/// (from the body), and Transfer-Encoding, which the client never uses. Native
/// and web clients both check, so that a request fails the same way on every
/// platform rather than sending a header twice or dropping it.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
checkRequestHeaders(std::span<const std::pair<std::string, std::string>> headers);

} // namespace pomdog::network::detail
