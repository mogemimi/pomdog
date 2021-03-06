// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Network/ForwardDeclarations.hpp"
#include "Pomdog/Network/HTTPMethod.hpp"
#include "Pomdog/Signals/Delegate.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

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

} // namespace Pomdog
