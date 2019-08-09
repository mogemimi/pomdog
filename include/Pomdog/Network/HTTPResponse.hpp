// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Network/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace Pomdog {

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
