// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/http_client.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <deque>
#include <memory>
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network {
class TrustStore;
} // namespace pomdog::network

namespace pomdog::network::detail {

/// HTTPClientEmscripten sends requests with fetch() of the browser.
///
/// The browser connects to servers, verifies them, and keeps connections.
/// JavaScript queues the result of each request, and `update()` takes them.
class HTTPClientEmscripten final : public HTTPClient {
public:
    explicit HTTPClientEmscripten(const HTTPClientOptions& options) noexcept;

    ~HTTPClientEmscripten() override;

    [[nodiscard]] u64
    send(HTTPRequest&& request) override;

    void cancel(u64 requestID) override;

    void update() override;

    [[nodiscard]] std::optional<HTTPEvent>
    pollEvent() override;

private:
    std::deque<HTTPEvent> events_;

    /// The events of requests that failed before fetch() started, which the
    /// next `update()` queues.
    std::vector<HTTPEvent> failures_;

    std::shared_ptr<TrustStore> trustStore_;
    f64 idleTimeoutMilliseconds_ = 0.0;
    u64 nextRequestID_ = 1;

    /// The ID of the JavaScript state of this client.
    i32 clientID_ = 0;
};

} // namespace pomdog::network::detail
