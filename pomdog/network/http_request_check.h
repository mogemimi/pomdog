// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/http_method.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <expected>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// RequestURL holds the parts of an absolute URL that a client needs to
/// connect to the server and to request the resource.
struct RequestURL final {
    /// The host and the port to connect to, such as "example.com:443" or
    /// "[::1]:80".
    std::string address;

    /// The value of the Host header, the host and the port without the
    /// default port of the scheme, such as "example.com" or "[::1]:8080".
    std::string host;

    /// The path and the query to request, such as "/chat?room=1", which is
    /// "/" when the URL has no path.
    std::string resource;

    /// True for secureScheme, such as https, false for scheme, such as http.
    bool secure = false;
};

/// Parses url, whose scheme is scheme or secureScheme in any case, such as
/// "http" and "https", with the default port 80 or 443.
///
/// Fails when url has another scheme, a fragment, user information, no host,
/// an invalid port, or a space or a control character, which could add lines
/// to the request header. Browsers fail the same URLs, except that they
/// remove or percent-encode spaces and control characters.
[[nodiscard]] std::expected<RequestURL, std::unique_ptr<Error>>
parseRequestURL(std::string_view url, std::string_view scheme, std::string_view secureScheme);

/// Checks the method of an `HTTPRequest` before the client sends it.
///
/// Fails for CONNECT and TRACE, the forbidden methods of the Fetch standard,
/// which browsers do not let fetch() send. Native and web clients both check,
/// so that a request fails the same way on every platform.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
checkRequestMethod(HTTPMethod method);

/// Checks the body of an `HTTPRequest` before the client sends it.
///
/// Fails for a GET or HEAD request with a body, which browsers do not let
/// fetch() send. Native and web clients both check, so that a request fails
/// the same way on every platform.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
checkRequestBody(HTTPMethod method, std::span<const u8> body);

/// Checks the headers of an `HTTPRequest` before the client sends it.
///
/// Fails when a name is not an HTTP token, when a value has a line break or a
/// null character, for the headers that the client sets itself (Host,
/// Connection, Content-Length, Transfer-Encoding, and Accept-Encoding), and
/// for the forbidden request-headers of the Fetch standard, which browsers do
/// not let fetch() send. Native and web clients both check, so that a request
/// fails the same way on every platform rather than sending a header twice or
/// the browser dropping it. The errors name the header, but not its value.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
checkRequestHeaders(std::span<const std::pair<std::string, std::string>> headers);

} // namespace pomdog::network::detail
