// Copyright mogemimi. Distributed under the MIT license.

#include "network_test_helper.h"
#include "network_test_options.h"
#include "tests/testing/testing.h"
#include "pomdog/basic/platform.h"
#include "pomdog/network/http_client.h"
#include "pomdog/network/http_method.h"
#include "pomdog/network/http_request.h"
#include "pomdog/network/http_response.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using namespace pomdog;
using namespace pomdog::network;
using pomdog::testing::getHTTPServerAddress;
using pomdog::testing::getHTTPSServerAddress;
using pomdog::testing::getUntrustedTLSServerAddress;

// NOTE: The tests talk to the local HTTP and HTTPS servers of
// network-test-runner, which answer the same paths. On the web, Node.js sends
// the requests with fetch(), and keeps connections by itself.

namespace {

/// Checks the protocol of response, which fetch() does not tell on the web.
void checkProtocol([[maybe_unused]] const HTTPResponse& response)
{
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    REQUIRE(response.protocol.empty());
#else
    REQUIRE(response.protocol == "HTTP/1.1");
#endif
}

std::string
toLower(std::string_view in)
{
    const auto asciiToLower = [](char c) -> char {
        if (c <= 'Z' && c >= 'A') {
            return c - ('Z' - 'z');
        }
        return c;
    };

    std::string s{in};
    std::transform(std::begin(s), std::end(s), std::begin(s), asciiToLower);
    return s;
}

/// Returns the value of the header named name, compared without case, or
/// nullopt when the response has none.
[[nodiscard]] std::optional<std::string>
findHeader(const HTTPResponse& response, std::string_view name)
{
    const auto lowerName = toLower(name);
    for (const auto& [key, value] : response.headers) {
        if (toLower(key) == lowerName) {
            return value;
        }
    }
    return std::nullopt;
}

[[nodiscard]] std::vector<u8>
toBytes(std::string_view text)
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    const auto view = std::span<const u8>{reinterpret_cast<const u8*>(text.data()), text.size()};
    POMDOG_CLANG_UNSAFE_BUFFER_END
    return std::vector<u8>(view.begin(), view.end());
}

[[nodiscard]] HTTPRequest
makeRequest(HTTPMethod method, std::string_view url)
{
    HTTPRequest request;
    request.method = method;
    request.url = url;
    return request;
}

[[nodiscard]] std::string
localURL(std::string_view path)
{
    return "http://" + std::string{getHTTPServerAddress()} + std::string{path};
}

[[nodiscard]] std::string
localHTTPSURL(std::string_view path)
{
    return "https://" + std::string{getHTTPSServerAddress()} + std::string{path};
}

[[nodiscard]] std::shared_ptr<HTTPClient>
makeClient(const HTTPClientOptions& options = HTTPClientOptions{})
{
    auto client = createHTTPClient(options);
    REQUIRE(client.has_value());
    REQUIRE(*client != nullptr);
    return std::move(*client);
}

/// Returns a client that trusts the root of the local HTTPS server.
[[nodiscard]] std::shared_ptr<HTTPClient>
makeTLSClient()
{
    HTTPClientOptions options;
    options.trustStore = loadTestTrustStore();
    return makeClient(options);
}

/// Updates client until an event arrives, and returns it.
[[nodiscard]] HTTPEvent
waitForEvent(HTTPClient& client)
{
    std::optional<HTTPEvent> received;
    const auto finished = runUntil([&] {
        client.update();
        received = client.pollEvent();
        return received.has_value();
    });
    REQUIRE(finished);
    REQUIRE(received.has_value());
    return std::move(*received);
}

/// Sends request and waits for its event.
[[nodiscard]] HTTPEvent
sendAndWait(HTTPClient& client, HTTPRequest&& request)
{
    const auto requestID = client.send(std::move(request));
    REQUIRE(requestID != 0);

    auto event = waitForEvent(client);
    REQUIRE(event.requestID == requestID);
    REQUIRE_FALSE(client.pollEvent().has_value());
    return event;
}

/// Sends request and returns its response. Fails the test when the request
/// fails.
[[nodiscard]] HTTPResponse
fetch(HTTPClient& client, HTTPRequest&& request)
{
    auto event = sendAndWait(client, std::move(request));
    if (!event.result) {
        FAIL("the request failed: " << describe(event.result.error()));
    }
    return std::move(*event.result);
}

/// Sends request, and returns the error of its failure. Fails the test when
/// the request succeeds.
[[nodiscard]] std::unique_ptr<Error>
fetchError(HTTPClient& client, HTTPRequest&& request)
{
    auto event = sendAndWait(client, std::move(request));
    REQUIRE_FALSE(event.result.has_value());
    REQUIRE(event.result.error() != nullptr);
    return std::move(event.result.error());
}

#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
/// Fetches a body of size bytes from url, checks it, and returns how many
/// updates the request took.
[[nodiscard]] int
countUpdatesToFetchBytes(HTTPClient& client, const std::string& url, std::size_t size)
{
    const auto requestID = client.send(makeRequest(HTTPMethod::Get, url + "/bytes/" + std::to_string(size)));
    REQUIRE(requestID != 0);

    int updateCount = 0;
    std::optional<HTTPEvent> received;
    const auto finished = runUntil([&] {
        client.update();
        ++updateCount;
        received = client.pollEvent();
        return received.has_value();
    });
    REQUIRE(finished);
    REQUIRE(received.has_value());
    REQUIRE(received->requestID == requestID);
    if (!received->result) {
        FAIL("the request failed: " << describe(received->result.error()));
    }
    REQUIRE(received->result->body.size() == size);
    REQUIRE(asText(received->result->body) == makeLetters(size));
    return updateCount;
}
#endif

} // namespace

TEST_CASE("HTTPClient sends a GET request to the local server")
{
    auto client = makeClient();

    auto request = makeRequest(HTTPMethod::Get, localURL("/echo?answer=42"));
    request.headers.emplace_back("X-Test", "hello");

    const auto response = fetch(*client, std::move(request));
    checkProtocol(response);
    REQUIRE(response.statusCode == 200);
    REQUIRE(response.status == "200 OK");
    REQUIRE(findHeader(response, "X-Method") == "GET");
    REQUIRE(findHeader(response, "X-Request-URI") == "/echo?answer=42");
    REQUIRE(findHeader(response, "X-Host") == std::string{getHTTPServerAddress()});
    REQUIRE(findHeader(response, "X-Header-X-Test") == "hello");
    REQUIRE(response.contentLength == 0);
    REQUIRE(response.body.empty());
}

TEST_CASE("HTTPClient sends a POST body to the local server")
{
    auto client = makeClient();

    auto request = makeRequest(HTTPMethod::Post, localURL("/echo"));
    request.headers.emplace_back("Content-Type", "text/plain");
    request.body = toBytes("hello, hi");

    const auto response = fetch(*client, std::move(request));
    REQUIRE(response.statusCode == 200);
    REQUIRE(findHeader(response, "X-Method") == "POST");
    REQUIRE(findHeader(response, "X-Header-Content-Length") == "9");
    REQUIRE(findHeader(response, "X-Header-Content-Type") == "text/plain");
    REQUIRE(response.contentLength == 9);
    REQUIRE(asText(response.body) == "hello, hi");
}

TEST_CASE("HTTPClient reads a body with Content-Length")
{
    auto client = makeClient();

    // NOTE: The body is larger than the buffer that a stream reads into.
    constexpr std::size_t size = 200000;
    const auto response = fetch(*client, makeRequest(HTTPMethod::Get, localURL("/bytes/" + std::to_string(size))));
    REQUIRE(response.statusCode == 200);
    REQUIRE_FALSE(response.chunkedTransferEncoding);
    REQUIRE(response.contentLength == size);
    REQUIRE(response.body.size() == size);

    bool matches = true;
    for (std::size_t i = 0; i < response.body.size(); ++i) {
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        matches = matches && (response.body[i] == static_cast<u8>('a' + (i % 26)));
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }
    REQUIRE(matches);
}

#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
// NOTE: Reading 2048 bytes per update would take at least 512 updates for
// 1 MiB. The local server sends faster than the test updates, so a stream that
// reads all that arrived needs far fewer. On the web, the browser reads.
TEST_CASE("HTTPClient receives a large body in a few updates")
{
    auto client = makeClient();

    constexpr std::size_t size = 1024 * 1024;
    const auto updateCount = countUpdatesToFetchBytes(*client, localURL(""), size);
    REQUIRE(updateCount < 512);
}
#endif

TEST_CASE("HTTPClient reads a chunked body")
{
    auto client = makeClient();

    constexpr int count = 1000;
    const auto response = fetch(*client, makeRequest(HTTPMethod::Get, localURL("/chunked/" + std::to_string(count))));
    REQUIRE(response.statusCode == 200);
    REQUIRE(response.chunkedTransferEncoding);
    REQUIRE(findHeader(response, "Transfer-Encoding") == "chunked");

    std::string expected;
    for (int i = 0; i < count; ++i) {
        expected += std::to_string(i) + ",";
    }
    REQUIRE(asText(response.body) == expected);
}

TEST_CASE("HTTPClient reports an error status as a response")
{
    auto client = makeClient();

    const auto response = fetch(*client, makeRequest(HTTPMethod::Get, localURL("/status/404")));
    REQUIRE(response.statusCode == 404);
    REQUIRE(response.status == "404 Not Found");
    REQUIRE(asText(response.body) == "Not Found");
}

#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
// NOTE: On the web, the browser decides which connection a request uses.
TEST_CASE("HTTPClient reuses a persistent connection")
{
    auto client = makeClient();

    const auto first = fetch(*client, makeRequest(HTTPMethod::Get, localURL("/echo")));
    const auto second = fetch(*client, makeRequest(HTTPMethod::Get, localURL("/echo")));
    REQUIRE(findHeader(first, "X-Connection-ID").has_value());
    REQUIRE(findHeader(first, "X-Connection-ID") == findHeader(second, "X-Connection-ID"));

    // NOTE: A request without a persistent connection closes its connection.
    auto request = makeRequest(HTTPMethod::Get, localURL("/echo"));
    request.persistentConnection = false;
    const auto third = fetch(*client, std::move(request));
    REQUIRE(findHeader(third, "X-Header-Connection") == "close");

    const auto fourth = fetch(*client, makeRequest(HTTPMethod::Get, localURL("/echo")));
    REQUIRE(findHeader(fourth, "X-Connection-ID").has_value());
    REQUIRE(findHeader(third, "X-Connection-ID") != findHeader(fourth, "X-Connection-ID"));
}
#endif

TEST_CASE("HTTPClient sends requests at the same time")
{
    auto client = makeClient();

    const auto first = client->send(makeRequest(HTTPMethod::Get, localURL("/delay/100")));
    const auto second = client->send(makeRequest(HTTPMethod::Get, localURL("/delay/100")));

    std::vector<HTTPEvent> events;
    const auto finished = runUntil([&] {
        client->update();
        while (auto event = client->pollEvent()) {
            events.push_back(std::move(*event));
        }
        return events.size() >= 2;
    });
    REQUIRE(finished);
    REQUIRE(events.size() == 2);

    std::vector<std::string> connectionIDs;
    for (auto& event : events) {
        REQUIRE(((event.requestID == first) || (event.requestID == second)));
        if (!event.result) {
            FAIL("the request failed: " << describe(event.result.error()));
        }
        REQUIRE(asText(event.result->body) == "delayed");
        const auto connectionID = findHeader(*event.result, "X-Connection-ID");
        REQUIRE(connectionID.has_value());
        connectionIDs.push_back(*connectionID);
    }
    REQUIRE(events[0].requestID != events[1].requestID);

    // NOTE: Each request opens its own connection, because no connection is
    // idle. HTTP/1.1 takes one request at a time on a connection, so Node.js
    // does the same on the web.
    REQUIRE(connectionIDs[0] != connectionIDs[1]);
}

TEST_CASE("Cancel HTTP Request")
{
    auto client = makeClient();

    auto request = makeRequest(HTTPMethod::Post, localURL("/delay/100"));
    request.body = toBytes("hello, hi");

    const auto requestID = client->send(std::move(request));
    REQUIRE(requestID != 0);

    client->cancel(requestID);

    // NOTE: No event arrives for the canceled request, even after the delay of
    // the server.
    const auto startTime = std::chrono::steady_clock::now();
    while ((std::chrono::steady_clock::now() - startTime) < std::chrono::milliseconds{300}) {
        client->update();
        REQUIRE_FALSE(client->pollEvent().has_value());
        sleepBriefly();
    }

    // NOTE: The client still sends later requests.
    const auto response = fetch(*client, makeRequest(HTTPMethod::Get, localURL("/echo")));
    REQUIRE(response.statusCode == 200);
}

TEST_CASE("HTTPClient reports an idle timeout as an error event")
{
    HTTPClientOptions options;
    options.idleTimeout = std::chrono::milliseconds{200};
    auto client = makeClient(options);

    auto event = sendAndWait(*client, makeRequest(HTTPMethod::Get, localURL("/delay/5000")));
    REQUIRE_FALSE(event.result.has_value());
    REQUIRE(event.result.error() != nullptr);
}

TEST_CASE("HTTPClient sends a GET request over https")
{
    auto client = makeTLSClient();

    const auto response = fetch(*client, makeRequest(HTTPMethod::Get, localHTTPSURL("/echo?answer=42")));
    checkProtocol(response);
    REQUIRE(response.statusCode == 200);
    REQUIRE(findHeader(response, "X-Method") == "GET");
    REQUIRE(findHeader(response, "X-Request-URI") == "/echo?answer=42");
    REQUIRE(findHeader(response, "X-Host") == std::string{getHTTPSServerAddress()});
}

TEST_CASE("HTTPClient sends a large POST body over https")
{
    auto client = makeTLSClient();

    // NOTE: The body is larger than a TLS record, which holds up to 16 KiB.
    const auto body = makeLetters(40000);
    auto request = makeRequest(HTTPMethod::Post, localHTTPSURL("/echo"));
    request.body = toBytes(body);

    const auto response = fetch(*client, std::move(request));
    REQUIRE(response.statusCode == 200);
    REQUIRE(findHeader(response, "X-Header-Content-Length") == std::to_string(body.size()));
    REQUIRE(asText(response.body) == body);
}

TEST_CASE("HTTPClient reads a chunked body over https")
{
    auto client = makeTLSClient();

    const auto response = fetch(*client, makeRequest(HTTPMethod::Get, localHTTPSURL("/chunked/3")));
    REQUIRE(response.statusCode == 200);
    REQUIRE(response.chunkedTransferEncoding);
    REQUIRE(asText(response.body) == "0,1,2,");
}

#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
// NOTE: See "HTTPClient receives a large body in a few updates".
TEST_CASE("HTTPClient receives a large body over https in a few updates")
{
    auto client = makeTLSClient();

    constexpr std::size_t size = 1024 * 1024;
    const auto updateCount = countUpdatesToFetchBytes(*client, localHTTPSURL(""), size);
    REQUIRE(updateCount < 512);
}
#endif

#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
// NOTE: On the web, the browser decides which connection a request uses.
TEST_CASE("HTTPClient reuses a persistent https connection")
{
    auto client = makeTLSClient();

    const auto first = fetch(*client, makeRequest(HTTPMethod::Get, localHTTPSURL("/echo")));
    const auto second = fetch(*client, makeRequest(HTTPMethod::Get, localHTTPSURL("/echo")));
    REQUIRE(findHeader(first, "X-Connection-ID").has_value());
    REQUIRE(findHeader(first, "X-Connection-ID") == findHeader(second, "X-Connection-ID"));
}
#endif

TEST_CASE("HTTPClient fails https requests without a trust store")
{
    auto client = makeClient();

    const auto err = fetchError(*client, makeRequest(HTTPMethod::Get, localHTTPSURL("/echo")));
    REQUIRE_MESSAGE(err->toString().find("trustStore") != std::string::npos, err->toString());

    // NOTE: http requests still work.
    const auto response = fetch(*client, makeRequest(HTTPMethod::Get, localURL("/echo")));
    REQUIRE(response.statusCode == 200);
}

TEST_CASE("HTTPClient fails https requests to a server that the trust store does not verify")
{
    auto client = makeTLSClient();

    const auto url = "https://" + std::string{getUntrustedTLSServerAddress()} + "/echo";
    [[maybe_unused]] const auto err = fetchError(*client, makeRequest(HTTPMethod::Get, url));

    // NOTE: On the web, the message comes from the browser.
#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
    REQUIRE_MESSAGE(err->toString().find("failed verification") != std::string::npos, err->toString());
#endif
}

TEST_CASE("HTTPClient reports a failed request as an event")
{
    auto client = makeClient();

    SUBCASE("no protocol scheme")
    {
        const auto requestID = client->send(makeRequest(HTTPMethod::Get, "example.com/index.html"));
        REQUIRE(requestID != 0);

        // NOTE: The event arrives after a later update().
        REQUIRE_FALSE(client->pollEvent().has_value());
        client->update();

        auto event = client->pollEvent();
        REQUIRE(event.has_value());
        REQUIRE(event->requestID == requestID);
        REQUIRE_FALSE(event->result.has_value());
        REQUIRE(event->result.error() != nullptr);
        REQUIRE_FALSE(client->pollEvent().has_value());
    }
    SUBCASE("unsupported protocol scheme")
    {
        const auto requestID = client->send(makeRequest(HTTPMethod::Get, "ftp://example.com/index.html"));
        REQUIRE(requestID != 0);

        client->update();

        auto event = client->pollEvent();
        REQUIRE(event.has_value());
        REQUIRE(event->requestID == requestID);
        REQUIRE_FALSE(event->result.has_value());
    }
}

TEST_CASE("HTTPClient request IDs are unique and not zero")
{
    auto client = makeClient();

    const auto first = client->send(makeRequest(HTTPMethod::Get, "invalid"));
    const auto second = client->send(makeRequest(HTTPMethod::Get, "invalid"));
    REQUIRE(first != 0);
    REQUIRE(second != 0);
    REQUIRE(first != second);

    client->update();

    auto firstEvent = client->pollEvent();
    REQUIRE(firstEvent.has_value());
    REQUIRE(firstEvent->requestID == first);

    auto secondEvent = client->pollEvent();
    REQUIRE(secondEvent.has_value());
    REQUIRE(secondEvent->requestID == second);
}

TEST_CASE("HTTPClient::cancel drops a queued event")
{
    auto client = makeClient();

    const auto canceled = client->send(makeRequest(HTTPMethod::Get, "invalid"));
    const auto kept = client->send(makeRequest(HTTPMethod::Get, "invalid"));
    client->update();

    client->cancel(canceled);

    auto event = client->pollEvent();
    REQUIRE(event.has_value());
    REQUIRE(event->requestID == kept);
    REQUIRE_FALSE(client->pollEvent().has_value());
}

TEST_CASE("HTTPClient fails a response body larger than maxResponseSize")
{
    HTTPClientOptions options;
    options.maxResponseSize = 1000;
    auto client = makeClient(options);

    SUBCASE("Content-Length")
    {
        const auto err = fetchError(*client, makeRequest(HTTPMethod::Get, localURL("/bytes/2000")));
        REQUIRE_MESSAGE(err->toString().find("larger than 1000 bytes") != std::string::npos, describe(err));
    }
    SUBCASE("chunked")
    {
        const auto err = fetchError(*client, makeRequest(HTTPMethod::Get, localURL("/chunked/500")));
        REQUIRE_MESSAGE(err->toString().find("larger than 1000 bytes") != std::string::npos, describe(err));
    }
    SUBCASE("a body within the limit")
    {
        const auto response = fetch(*client, makeRequest(HTTPMethod::Get, localURL("/bytes/1000")));
        REQUIRE(response.body.size() == 1000);
    }
}

TEST_CASE("HTTPClient reads no body for a HEAD request")
{
    auto client = makeClient();

    // NOTE: The response has Content-Length, but no body follows it.
    const auto response = fetch(*client, makeRequest(HTTPMethod::Head, localURL("/bytes/100")));
    REQUIRE(response.statusCode == 200);
    REQUIRE(response.contentLength == 100);
    REQUIRE(response.body.empty());
}

TEST_CASE("HTTPClient reads a body that ends when the server closes the connection")
{
    SUBCASE("http")
    {
        auto client = makeClient();
        const auto response = fetch(*client, makeRequest(HTTPMethod::Get, localURL("/close/3000")));
        REQUIRE(response.statusCode == 200);
        REQUIRE(asText(response.body) == makeLetters(3000));
    }
    SUBCASE("https")
    {
        auto client = makeTLSClient();
        const auto response = fetch(*client, makeRequest(HTTPMethod::Get, localHTTPSURL("/close/3000")));
        REQUIRE(response.statusCode == 200);
        REQUIRE(asText(response.body) == makeLetters(3000));
    }
}

TEST_CASE("HTTPClient cancels a request that still connects at once")
{
    // NOTE: 192.0.2.1 (TEST-NET-1) never answers, so the connection would run
    // until the timeout.
    HTTPClientOptions options;
    options.connectTimeout = std::chrono::seconds{10};
    auto client = makeClient(options);

    const auto requestID = client->send(makeRequest(HTTPMethod::Get, "http://192.0.2.1:9/"));
    client->update();

    const auto startTime = std::chrono::steady_clock::now();
    client->cancel(requestID);
    client->update();
    REQUIRE_FALSE(client->pollEvent().has_value());
    client.reset();
    REQUIRE((std::chrono::steady_clock::now() - startTime) < std::chrono::seconds{2});
}
