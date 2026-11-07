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

    /// The response body, without content coding. Native clients ask for none,
    /// and fail a response whose body has one, such as gzip. On the web, the
    /// browser negotiates the coding and decodes the body.
    std::vector<u8> body;

    /// The URL of the response without its fragment, which is the URL of the
    /// last request after redirects.
    std::string url;

    /// The value of the "Content-Length" header, or zero without it or with
    /// chunked encoding. A response to HEAD, or with status 204 or 304, keeps
    /// the value but has no body. On the web, the browser decodes compressed
    /// bodies, so the body may be longer.
    std::size_t contentLength = 0;

    /// True if "Transfer-Encoding" has a "chunked" directive, false otherwise.
    bool chunkedTransferEncoding = false;
};

} // namespace pomdog::network
