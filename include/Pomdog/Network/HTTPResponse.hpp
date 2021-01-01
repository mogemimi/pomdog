// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Network/ForwardDeclarations.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

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

} // namespace Pomdog
