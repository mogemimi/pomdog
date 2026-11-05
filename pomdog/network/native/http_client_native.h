// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/http_client.h"
#include "pomdog/network/stream.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <deque>
#include <expected>
#include <memory>
#include <optional>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// HTTPClientNative is an HTTP/1.1 client built on TCP and TLS streams.
///
/// It keeps a connection open after a response, so that a later request to
/// the same server reuses it.
class HTTPClientNative final : public HTTPClient {
public:
    explicit HTTPClientNative(const HTTPClientOptions& options) noexcept;

    ~HTTPClientNative() override;

    [[nodiscard]] u64
    send(HTTPRequest&& request) override;

    void cancel(u64 requestID) override;

    void update() override;

    [[nodiscard]] std::optional<HTTPEvent>
    pollEvent() override;

private:
    struct Session;

    /// IdleConnection is an open connection that waits for the next request.
    struct IdleConnection final {
        /// The scheme, host, and port of the server, such as "https://example.com:443".
        std::string key;
        std::shared_ptr<Stream> stream;
    };

    /// Prepares the request bytes and the connection of session.
    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    startSession(Session& session, const HTTPRequest& request);

    /// Advances session, and queues its event when it ends.
    void updateSession(Session& session);

    /// Queues the event of session with result, and ends session.
    void complete(Session& session, std::expected<HTTPResponse, std::unique_ptr<Error>>&& result);

private:
    std::vector<std::unique_ptr<Session>> sessions_;
    std::vector<IdleConnection> idleConnections_;
    std::deque<HTTPEvent> events_;
    StreamOptions streamOptions_;
    std::shared_ptr<TrustStore> trustStore_;
    std::size_t maxResponseSize_ = 0;
    u64 nextRequestID_ = 1;
};

} // namespace pomdog::network::detail
