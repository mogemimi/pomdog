// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/forward_declarations.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <expected>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// Returns true if statusCode is a redirect status of the Fetch standard: 301,
/// 302, 303, 307, or 308.
[[nodiscard]] bool
isRedirectStatus(i32 statusCode) noexcept;

/// Returns url without its fragment, the part from "#".
[[nodiscard]] std::string_view
removeURLFragment(std::string_view url) noexcept;

/// Resolves reference, such as the value of a Location header, against base,
/// an absolute URL, as RFC 3986, section 5.2, describes. Fails when reference
/// has a space or a control character.
[[nodiscard]] std::expected<std::string, std::unique_ptr<Error>>
resolveURL(std::string_view base, std::string_view reference);

/// Returns the origin of url, an absolute http or https URL, with its scheme
/// and host in lowercase and its port, such as "https://example.com:443".
[[nodiscard]] std::string
getURLOrigin(std::string_view url);

/// Returns the request that follows response, a redirect of request, by the
/// rules of fetch(), or nothing when the response has no Location header.
///
/// Fails when the location is not a valid URL, is not http or https, or goes
/// from https to http.
[[nodiscard]] std::expected<std::optional<HTTPRequest>, std::unique_ptr<Error>>
makeRedirectRequest(const HTTPRequest& request, const HTTPResponse& response);

} // namespace pomdog::network::detail
