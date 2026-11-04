// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/network/http_request.h"
#include "pomdog/network/http_response.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <chrono>
#include <expected>
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network {
class TrustStore;
} // namespace pomdog::network

namespace pomdog::network {

// NOTE: An event owns its error, so it is move-only. MSVC reports the
// implicitly deleted copy operations unless these warnings are off.
POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4625)
POMDOG_MSVC_SUPPRESS_WARNING(4626)
/// Reports the end of a request that `HTTPClient::send()` started.
struct HTTPEvent final {
    /// The request that ended.
    u64 requestID = 0;

    /// The response, or why the request failed, such as an invalid URL, a
    /// failed connection, or a timeout. A response with an error status, such
    /// as 404, is a response rather than a failure.
    std::expected<HTTPResponse, std::unique_ptr<Error>> result;
};
POMDOG_MSVC_SUPPRESS_WARNING_POP

/// Sets how an `HTTPClient` connects and when it gives up.
struct HTTPClientOptions final {
    /// Limits how long connecting to a server may take. A TLS handshake gets
    /// the same time again. Name resolution is not limited.
    Duration connectTimeout = std::chrono::seconds{5};

    /// Fails a request whose connection sends and receives nothing for this
    /// long, and closes kept connections that stay idle for this long. Zero
    /// sets no limit.
    Duration idleTimeout = std::chrono::seconds{5};

    /// Verifies the servers of https requests, as `connectTLSStream()` does.
    /// Without it, https requests fail, and http requests still work.
    std::shared_ptr<TrustStore> trustStore;
};

/// Sends HTTP requests and receives their responses.
///
/// Requests are asynchronous. `send()` returns an ID, and the request's result
/// arrives as an event after a later `update()`, even when the request fails
/// at once, such as for an invalid URL. Events wait in a queue until
/// `pollEvent()` takes them, so take them every frame:
/// ```cpp
/// client->update();
/// while (auto event = client->pollEvent()) {
///     if (event->requestID != pendingRequest) {
///         continue;
///     }
///     pendingRequest = 0;
///     if (!event->result) {
///         showError(*event->result.error());
///         continue;
///     }
///     body = std::move(event->result->body);
/// }
/// ```
///
/// A client is not thread-safe. Use it from the thread that runs the game loop.
class POMDOG_EXPORT HTTPClient {
public:
    HTTPClient() noexcept;

    HTTPClient(const HTTPClient&) = delete;
    HTTPClient& operator=(const HTTPClient&) = delete;

    virtual ~HTTPClient();

    /// Starts sending request.
    ///
    /// Returns the request's ID, which is never zero. Its event arrives after
    /// a later `update()`.
    [[nodiscard]] virtual u64
    send(HTTPRequest&& request) = 0;

    /// Stops the request with requestID. No event for that request arrives
    /// after this call, including one that already waits in the queue. Does
    /// nothing for an ID whose event was already taken.
    virtual void
    cancel(u64 requestID) = 0;

    /// Advances the requests in flight and queues the events of those that
    /// ended. Call it once per frame, before taking events.
    virtual void
    update() = 0;

    /// Takes the oldest queued event, or returns nullopt when none waits.
    [[nodiscard]] virtual std::optional<HTTPEvent>
    pollEvent() = 0;
};

/// Creates an HTTP client.
///
/// The client sends requests over its own sockets, with Mbed TLS for HTTPS.
/// On the web it returns an error, because Pomdog does not implement the
/// client for the browser yet.
[[nodiscard]] POMDOG_EXPORT std::expected<std::shared_ptr<HTTPClient>, std::unique_ptr<Error>>
createHTTPClient(const HTTPClientOptions& options);

} // namespace pomdog::network
