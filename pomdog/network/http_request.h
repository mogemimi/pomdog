// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/http_method.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network {

/// HTTPRequest represents an HTTP request that `HTTPClient::send()` sends.
struct HTTPRequest final {
    /// The method for the request.
    HTTPMethod method = HTTPMethod::Get;

    /// The URL for the request, such as "https://example.com/index.html".
    std::string url;

    /// The request headers as name and value pairs, sent in this order.
    std::vector<std::pair<std::string, std::string>> headers;

    /// The request body.
    std::vector<u8> body;

    /// True to keep the connection open after the response, so that a later
    /// request to the same server can reuse it.
    bool persistentConnection = true;
};

} // namespace pomdog::network
