// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/network/forward_declarations.hpp"

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
    std::string Status;

    /// e.g. 200
    int StatusCode;

    /// e.g. HTTP/1.1
    std::string Protocol;

    std::vector<std::pair<std::string, std::string>> Header;

    std::vector<char> Body;

    std::size_t ContentLength = 0;

    /// True if "Transfer-Encoding" has a "chunked" directive, false otherwise.
    bool ChunkedTransferEncoding = false;

    std::shared_ptr<HTTPRequest> Request;
};

} // namespace pomdog
