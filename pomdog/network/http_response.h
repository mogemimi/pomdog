// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// An HTTPResponse represents an HTTP response from the server to the client.
class POMDOG_EXPORT HTTPResponse final {
public:
    /// e.g. "200 OK"
    std::string status;

    /// e.g. 200
    i32 statusCode;

    /// e.g. HTTP/1.1
    std::string protocol;

    std::vector<std::pair<std::string, std::string>> header;

    std::vector<char> body;

    std::size_t contentLength = 0;

    /// True if "Transfer-Encoding" has a "chunked" directive, false otherwise.
    bool chunkedTransferEncoding = false;

    std::shared_ptr<HTTPRequest> request;
};

} // namespace pomdog
