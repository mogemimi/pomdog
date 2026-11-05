// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network {

/// HTTPResponse represents an HTTP response from the server to the client.
struct HTTPResponse final {
    /// The status code and its reason phrase, such as "200 OK". On the web, a
    /// response over HTTP/2 has no reason phrase, such as "200".
    std::string status;

    /// The status code, such as 200.
    i32 statusCode = 0;

    /// The protocol of the response, such as "HTTP/1.1". Empty on the web,
    /// where fetch() does not tell it.
    std::string protocol;

    /// The response headers as name and value pairs, in the order received.
    /// On the web, the names are lowercase and sorted, and the browser hides
    /// some headers of a response from another origin.
    std::vector<std::pair<std::string, std::string>> headers;

    /// The response body.
    std::vector<u8> body;

    /// The value of the "Content-Length" header, or zero without it. On the
    /// web, the browser decodes compressed bodies, so the body may be longer.
    std::size_t contentLength = 0;

    /// True if "Transfer-Encoding" has a "chunked" directive, false otherwise.
    bool chunkedTransferEncoding = false;
};

} // namespace pomdog::network
