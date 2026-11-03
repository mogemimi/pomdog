// Copyright mogemimi. Distributed under the MIT license.

#include "network_test_helper.h"
#include "network_test_options.h"
#include "tests/testing/testing.h"
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
#include <thread>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using namespace pomdog;
using namespace pomdog::network;
using pomdog::testing::getHTTPServerAddress;

// NOTE: The http tests talk to the local HTTP server of network-test-runner.
// The https tests use public servers, because the embedded root certificates
// verify no local certificate. httpbin.org echoes each request as JSON. Its
// formatting, header names, and header order change over time, so the tests
// look headers up by name and JSON members by key and value instead of
// comparing whole texts.

namespace {

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

[[nodiscard]] std::string_view
trimLeadingSpaces(std::string_view text) noexcept
{
    while (!text.empty() && ((text.front() == ' ') || (text.front() == '\n') || (text.front() == '\r') || (text.front() == '\t'))) {
        text.remove_prefix(1);
    }
    return text;
}

/// Returns true when json has a member named key whose value is the JSON text
/// value, such as `"hello"` or `42`, at any depth. Spaces around the colon do
/// not matter.
[[nodiscard]] bool
hasMember(std::string_view json, std::string_view key, std::string_view value)
{
    const auto quotedKey = "\"" + std::string{key} + "\"";
    for (auto position = json.find(quotedKey); position != std::string_view::npos; position = json.find(quotedKey, position + 1)) {
        auto rest = trimLeadingSpaces(json.substr(position + quotedKey.size()));
        if (!rest.starts_with(':')) {
            continue;
        }
        rest = trimLeadingSpaces(rest.substr(1));
        if (!rest.starts_with(value)) {
            continue;
        }
        const auto after = trimLeadingSpaces(rest.substr(value.size()));
        if (after.empty() || after.starts_with(',') || after.starts_with('}')) {
            return true;
        }
    }
    return false;
}

/// Checks the status line and the headers of an httpbin.org response, and
/// returns its JSON body, or nullopt when the server did not answer with 200.
[[nodiscard]] std::optional<std::string>
checkJSONResponse(const HTTPResponse& response)
{
    REQUIRE((response.protocol == "HTTP/1.1" || response.protocol == "HTTP/1.0"));

    // NOTE: Accept any valid HTTP status code (server may return errors like 502)
    REQUIRE(response.statusCode >= 100);
    REQUIRE(response.statusCode < 600);
    REQUIRE(!response.status.empty());
    REQUIRE(!response.headers.empty());
    if (response.statusCode != 200) {
        MESSAGE("the server answered " << response.statusCode << " " << response.status);
        return std::nullopt;
    }

    REQUIRE(findHeader(response, "Date").has_value());
    REQUIRE(findHeader(response, "Content-Type") == "application/json");
    REQUIRE(findHeader(response, "Content-Length") == std::to_string(response.contentLength));
    REQUIRE(response.contentLength == response.body.size());
    return std::string{reinterpret_cast<const char*>(response.body.data()), response.body.size()};
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

[[nodiscard]] std::shared_ptr<HTTPClient>
makeClient(const HTTPClientOptions& options = HTTPClientOptions{})
{
    auto client = createHTTPClient(options);
    REQUIRE(client.has_value());
    REQUIRE(*client != nullptr);
    return std::move(*client);
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

/// Sends request to the local server and returns its response.
[[nodiscard]] HTTPResponse
fetchLocal(HTTPClient& client, HTTPRequest&& request)
{
    auto event = sendAndWait(client, std::move(request));
    if (!event.result) {
        FAIL("the request failed: " << describe(event.result.error()));
    }
    return std::move(*event.result);
}

/// Sends request to a public server and waits for its response. Returns
/// nullopt when the request failed, after reporting why.
[[nodiscard]] std::optional<HTTPResponse>
fetch(HTTPClient& client, HTTPRequest&& request)
{
    auto event = sendAndWait(client, std::move(request));
    if (!event.result) {
        REQUIRE(event.result.error() != nullptr);
        reportNetworkError(*event.result.error());
        return std::nullopt;
    }
    return std::move(*event.result);
}

} // namespace

TEST_CASE("HTTPClient sends a GET request to the local server")
{
    auto client = makeClient();

    auto request = makeRequest(HTTPMethod::Get, localURL("/echo?answer=42"));
    request.headers.emplace_back("X-Test", "hello");

    const auto response = fetchLocal(*client, std::move(request));
    REQUIRE(response.protocol == "HTTP/1.1");
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

    const auto response = fetchLocal(*client, std::move(request));
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

    // NOTE: The body is larger than what a stream reads in one update.
    constexpr std::size_t size = 20000;
    const auto response = fetchLocal(*client, makeRequest(HTTPMethod::Get, localURL("/bytes/" + std::to_string(size))));
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

TEST_CASE("HTTPClient reads a chunked body")
{
    auto client = makeClient();

    constexpr int count = 1000;
    const auto response = fetchLocal(*client, makeRequest(HTTPMethod::Get, localURL("/chunked/" + std::to_string(count))));
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

    const auto response = fetchLocal(*client, makeRequest(HTTPMethod::Get, localURL("/status/404")));
    REQUIRE(response.statusCode == 404);
    REQUIRE(response.status == "404 Not Found");
    REQUIRE(asText(response.body) == "Not Found");
}

TEST_CASE("HTTPClient reuses a persistent connection")
{
    auto client = makeClient();

    const auto first = fetchLocal(*client, makeRequest(HTTPMethod::Get, localURL("/echo")));
    const auto second = fetchLocal(*client, makeRequest(HTTPMethod::Get, localURL("/echo")));
    REQUIRE(findHeader(first, "X-Connection-ID").has_value());
    REQUIRE(findHeader(first, "X-Connection-ID") == findHeader(second, "X-Connection-ID"));

    // NOTE: A request without a persistent connection closes its connection.
    auto request = makeRequest(HTTPMethod::Get, localURL("/echo"));
    request.persistentConnection = false;
    const auto third = fetchLocal(*client, std::move(request));
    REQUIRE(findHeader(third, "X-Header-Connection") == "close");

    const auto fourth = fetchLocal(*client, makeRequest(HTTPMethod::Get, localURL("/echo")));
    REQUIRE(findHeader(fourth, "X-Connection-ID").has_value());
    REQUIRE(findHeader(third, "X-Connection-ID") != findHeader(fourth, "X-Connection-ID"));
}

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

    // NOTE: Each request opens its own connection, because no connection is idle.
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
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // NOTE: The client still sends later requests.
    const auto response = fetchLocal(*client, makeRequest(HTTPMethod::Get, localURL("/echo")));
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

TEST_CASE("Transfer-Encoding has a 'chunked' operation.")
{
    auto client = makeClient();

    auto request = makeRequest(HTTPMethod::Get, "https://www.google.com");
    request.headers.emplace_back("User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X x.y; rv:42.0) Gecko/20100101 Firefox/42.0");

    auto response = fetch(*client, std::move(request));
    if (!response) {
        return;
    }

    // NOTE: Verify HTTP client correctly parses the response protocol
    REQUIRE((response->protocol == "HTTP/1.1" || response->protocol == "HTTP/1.0"));

    // NOTE: Accept any valid HTTP status code (server may return errors like 502)
    REQUIRE(response->statusCode >= 100);
    REQUIRE(response->statusCode < 600);
    REQUIRE(!response->status.empty());

    // NOTE: For successful responses, verify body is present
    if (response->statusCode == 200) {
        REQUIRE(!response->body.empty());
    }
}

TEST_CASE("HTTPClient Get Secure")
{
    auto client = makeClient();

    auto response = fetch(*client, makeRequest(HTTPMethod::Get, "https://httpbin.org/get"));
    if (!response) {
        return;
    }

    if (const auto json = checkJSONResponse(*response); json) {
        REQUIRE(hasMember(*json, "args", "{}"));
        REQUIRE(hasMember(*json, "Host", "\"httpbin.org\""));
        REQUIRE(hasMember(*json, "url", "\"https://httpbin.org/get\""));
    }
}

TEST_CASE("HTTPClient Post Secure")
{
    auto client = makeClient();

    auto request = makeRequest(HTTPMethod::Post, "https://httpbin.org/post");
    request.body = toBytes("hello, hi");

    auto response = fetch(*client, std::move(request));
    if (!response) {
        return;
    }

    if (const auto json = checkJSONResponse(*response); json) {
        REQUIRE(hasMember(*json, "data", "\"hello, hi\""));
        REQUIRE(hasMember(*json, "Content-Length", "\"9\""));
        REQUIRE(hasMember(*json, "Host", "\"httpbin.org\""));
        REQUIRE(hasMember(*json, "json", "null"));
        REQUIRE(hasMember(*json, "url", "\"https://httpbin.org/post\""));
    }
}

TEST_CASE("HTTPClient Post JSON")
{
    auto client = makeClient();

    auto request = makeRequest(HTTPMethod::Post, "https://httpbin.org/post");
    request.headers.emplace_back("Content-Type", "application/json");
    request.body = toBytes(R"({"answer":42, "text": "hello"})");

    auto response = fetch(*client, std::move(request));
    if (!response) {
        return;
    }

    if (const auto json = checkJSONResponse(*response); json) {
        REQUIRE(hasMember(*json, "Content-Length", "\"30\""));
        REQUIRE(hasMember(*json, "Content-Type", "\"application/json\""));
        REQUIRE(hasMember(*json, "answer", "42"));
        REQUIRE(hasMember(*json, "text", "\"hello\""));
        REQUIRE(hasMember(*json, "url", "\"https://httpbin.org/post\""));
    }
}

TEST_CASE("multiple connection")
{
    auto client = makeClient();

    // NOTE: The second request reuses the connection of the first one.
    for (int i = 0; i < 2; ++i) {
        auto response = fetch(*client, makeRequest(HTTPMethod::Get, "https://httpbin.org/get"));
        if (!response) {
            return;
        }

        // NOTE: Verify HTTP client correctly parses the response protocol
        REQUIRE((response->protocol == "HTTP/1.1" || response->protocol == "HTTP/1.0"));

        // NOTE: Accept any valid HTTP status code (server may return errors like 502)
        REQUIRE(response->statusCode >= 100);
        REQUIRE(response->statusCode < 600);
        REQUIRE(!response->status.empty());
    }
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
