// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Network/ForwardDeclarations.hpp"
#include "Pomdog/Signals/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

/// An HTTPClient represents an HTTP Client for sending HTTP requests and receiving HTTP responses.
class POMDOG_EXPORT HTTPClient final {
public:
    explicit HTTPClient(IOService* service);

    HTTPClient(const HTTPClient&) = delete;
    HTTPClient& operator=(const HTTPClient&) = delete;

    ~HTTPClient();

    /// Sends an HTTP request and returns an HTTP response.
    [[nodiscard]] std::unique_ptr<Error>
    Do(const std::shared_ptr<HTTPRequest>& req);

    /// Sends a GET request to the specified URL.
    [[nodiscard]] std::tuple<Connection, std::unique_ptr<Error>>
    Get(const std::string& url,
        std::function<void(std::unique_ptr<HTTPResponse>&&, const std::unique_ptr<Error>&)>&& callback);

    /// Sends a POST request to the specified URL.
    [[nodiscard]] std::tuple<Connection, std::unique_ptr<Error>>
    Post(const std::string& url,
        const std::string& contentType,
        std::vector<char>&& body,
        std::function<void(std::unique_ptr<HTTPResponse>&&, const std::unique_ptr<Error>&)>&& callback);

    /// Aborts the specified request immediately.
    [[nodiscard]] std::unique_ptr<Error>
    CancelRequest(const std::shared_ptr<HTTPRequest>& req);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
