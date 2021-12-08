// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/network/forward_declarations.hpp"
#include "pomdog/network/http_method.hpp"
#include "pomdog/signals/delegate.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// An HTTPRequest represents an HTTP request for a client connection.
class POMDOG_EXPORT HTTPRequest final {
public:
    static std::shared_ptr<HTTPRequest>
    Create(HTTPMethod method, const std::string& url);

    void AddHeader(const std::string& key, const std::string& value);

public:
    /// The URL for the request (e.g. "https://example.com")
    std::string URL;

    /// The request headers
    std::vector<std::pair<std::string, std::string>> Headers;

    /// The request body
    std::vector<char> Body;

    /// Callback function that will be called on request completion or error caused
    Delegate<void(std::unique_ptr<HTTPResponse>&&, const std::unique_ptr<Error>&)> OnCompleted;

    /// The method for the request
    HTTPMethod Method;

    bool PersistentConnection = true;
};

} // namespace pomdog
