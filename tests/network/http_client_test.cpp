// Copyright mogemimi. Distributed under the MIT license.

#include "network_test_helper.h"
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

// NOTE: httpbin.org echoes each request as JSON. Its formatting, header names,
// and header order change over time, so the tests look headers up by name and
// JSON members by key and value instead of comparing whole texts.

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

[[nodiscard]] std::shared_ptr<HTTPClient>
makeClient()
{
    auto client = createHTTPClient(HTTPClientOptions{});
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

/// Sends request and waits for its response. Returns nullopt when the request
/// failed, after reporting why.
[[nodiscard]] std::optional<HTTPResponse>
fetch(HTTPClient& client, HTTPRequest&& request)
{
    const auto requestID = client.send(std::move(request));
    REQUIRE(requestID != 0);

    auto event = waitForEvent(client);
    REQUIRE(event.requestID == requestID);
    REQUIRE_FALSE(client.pollEvent().has_value());

    if (!event.result) {
        REQUIRE(event.result.error() != nullptr);
        reportNetworkError(*event.result.error());
        return std::nullopt;
    }
    return std::move(*event.result);
}

} // namespace

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

TEST_CASE("HTTPClient Get")
{
    auto client = makeClient();

    auto response = fetch(*client, makeRequest(HTTPMethod::Get, "http://httpbin.org/get"));
    if (!response) {
        return;
    }

    if (const auto json = checkJSONResponse(*response); json) {
        REQUIRE(hasMember(*json, "args", "{}"));
        REQUIRE(hasMember(*json, "Host", "\"httpbin.org\""));
        REQUIRE(hasMember(*json, "url", "\"http://httpbin.org/get\""));
    }
}

TEST_CASE("HTTPClient Post")
{
    auto client = makeClient();

    auto request = makeRequest(HTTPMethod::Post, "http://httpbin.org/post");
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
        REQUIRE(hasMember(*json, "url", "\"http://httpbin.org/post\""));
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

TEST_CASE("Cancel HTTP Request")
{
    auto client = makeClient();

    auto request = makeRequest(HTTPMethod::Post, "http://httpbin.org/post");
    request.body = toBytes("hello, hi");

    const auto requestID = client->send(std::move(request));
    REQUIRE(requestID != 0);

    client->cancel(requestID);

    // NOTE: No event arrives for the canceled request.
    for (int i = 0; i < 500; ++i) {
        client->update();
        REQUIRE_FALSE(client->pollEvent().has_value());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
