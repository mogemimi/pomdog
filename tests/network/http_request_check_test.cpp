// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/network/http_request_check.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using pomdog::network::HTTPMethod;
using pomdog::network::detail::checkRequestBody;
using pomdog::network::detail::checkRequestHeaders;
using pomdog::network::detail::checkRequestMethod;
using pomdog::network::detail::parseRequestURL;

namespace {

/// Returns the error message of checking one header, or an empty string when
/// the check passes.
[[nodiscard]] std::string
check(std::string_view name, std::string_view value)
{
    const std::vector<std::pair<std::string, std::string>> headers = {{std::string{name}, std::string{value}}};
    auto result = checkRequestHeaders(headers);
    return result ? std::string{} : result.error()->toString();
}

} // namespace

TEST_CASE("parseRequestURL splits http and ws URLs")
{
    auto url = parseRequestURL("http://example.com/index.html?page=2", "http", "https");
    REQUIRE(url.has_value());
    REQUIRE_FALSE(url->secure);
    REQUIRE(url->address == "example.com:80");
    REQUIRE(url->host == "example.com");
    REQUIRE(url->resource == "/index.html?page=2");

    url = parseRequestURL("HTTPS://example.com:8443", "http", "https");
    REQUIRE(url.has_value());
    REQUIRE(url->secure);
    REQUIRE(url->address == "example.com:8443");
    REQUIRE(url->host == "example.com:8443");
    REQUIRE(url->resource == "/");

    // NOTE: The Host header leaves out the default port of the scheme (RFC
    // 9110, section 7.2).
    url = parseRequestURL("https://example.com:443/", "http", "https");
    REQUIRE(url.has_value());
    REQUIRE(url->address == "example.com:443");
    REQUIRE(url->host == "example.com");

    url = parseRequestURL("http://example.com?answer=42", "http", "https");
    REQUIRE(url.has_value());
    REQUIRE(url->host == "example.com");
    REQUIRE(url->resource == "/?answer=42");

    url = parseRequestURL("ws://example.com/chat?room=1", "ws", "wss");
    REQUIRE(url.has_value());
    REQUIRE_FALSE(url->secure);
    REQUIRE(url->address == "example.com:80");
    REQUIRE(url->resource == "/chat?room=1");

    url = parseRequestURL("wss://example.com?token=abc", "ws", "wss");
    REQUIRE(url.has_value());
    REQUIRE(url->secure);
    REQUIRE(url->address == "example.com:443");
    REQUIRE(url->resource == "/?token=abc");

    url = parseRequestURL("http://[::1]:8080/socket", "http", "https");
    REQUIRE(url.has_value());
    REQUIRE(url->address == "[::1]:8080");
    REQUIRE(url->host == "[::1]:8080");
    REQUIRE(url->resource == "/socket");

    url = parseRequestURL("ws://[::1]", "ws", "wss");
    REQUIRE(url.has_value());
    REQUIRE(url->address == "[::1]:80");
    REQUIRE(url->host == "[::1]");
}

TEST_CASE("parseRequestURL fails URLs that could change the request header")
{
    for (const auto url : std::to_array<std::string_view>({
             "http://example.com/\r\nX-Injected: 1",
             "http://example.com\r\nX-Injected: 1/",
             "http://example.com/a\nb",
             "http://example.com/a b",
             "http://example.com/\x7f",
             "http://user:password@example.com/",
         })) {
        CAPTURE(url);
        REQUIRE_FALSE(parseRequestURL(url, "http", "https").has_value());
    }
}

TEST_CASE("parseRequestURL fails URLs that browsers fail")
{
    for (const auto url : std::to_array<std::string_view>({
             "http://example.com/",
             "example.com/chat",
             "ws://example.com/#fragment",
             "ws://user:password@example.com/",
             "ws:///path",
             "ws://:80/",
             "ws://[::1/",
             "ws://[::1]x/",
             "ws://example.com:http/",
             "ws://example.com:65536/",
             "ws://example.com/a b",
             "ws://example.com/\x7f",
         })) {
        CAPTURE(url);
        REQUIRE_FALSE(parseRequestURL(url, "ws", "wss").has_value());
    }
}

TEST_CASE("checkRequestMethod fails the methods that fetch() forbids")
{
    for (const auto method : {HTTPMethod::Get, HTTPMethod::Head, HTTPMethod::Post, HTTPMethod::Put, HTTPMethod::Delete}) {
        REQUIRE(checkRequestMethod(method).has_value());
    }

    auto result = checkRequestMethod(HTTPMethod::Connect);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error()->toString().find("CONNECT") != std::string::npos);

    result = checkRequestMethod(HTTPMethod::Trace);
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error()->toString().find("TRACE") != std::string::npos);
}

TEST_CASE("checkRequestBody fails a body on a GET or HEAD request")
{
    const auto body = std::to_array<pomdog::u8>({1, 2, 3});

    for (const auto method : {HTTPMethod::Get, HTTPMethod::Head}) {
        auto result = checkRequestBody(method, body);
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error()->toString().find("must not have a body") != std::string::npos);
    }
    for (const auto method : {HTTPMethod::Get, HTTPMethod::Head, HTTPMethod::Post, HTTPMethod::Put, HTTPMethod::Delete}) {
        REQUIRE(checkRequestBody(method, {}).has_value());
    }
    for (const auto method : {HTTPMethod::Post, HTTPMethod::Put, HTTPMethod::Delete}) {
        REQUIRE(checkRequestBody(method, body).has_value());
    }
}

TEST_CASE("checkRequestHeaders accepts headers that fetch() may send")
{
    REQUIRE(check("Authorization", "Bearer token") == "");
    REQUIRE(check("X-API-Key", "key") == "");
    REQUIRE(check("Content-Type", "application/json") == "");
    REQUIRE(check("Accept", "*/*") == "");
    REQUIRE(check("X-HTTP-Method-Override", "PUT") == "");
}

TEST_CASE("checkRequestHeaders fails the headers that the client sets itself")
{
    constexpr auto names = std::to_array<std::string_view>({"Host", "Connection", "Content-Length", "Transfer-Encoding", "accept-encoding", "COOKIE"});
    for (const auto name : names) {
        const auto message = check(name, "x");
        REQUIRE_MESSAGE(message.find(name) != std::string::npos, message);
    }
}

TEST_CASE("checkRequestHeaders fails the forbidden request-headers of fetch()")
{
    constexpr auto names = std::to_array<std::string_view>({
        "Accept-Charset",
        "Access-Control-Request-Headers",
        "Access-Control-Request-Method",
        "Cookie2",
        "Date",
        "DNT",
        "Expect",
        "Keep-Alive",
        "origin",
        "Referer",
        "Set-Cookie",
        "TE",
        "Trailer",
        "Upgrade",
        "Via",
        "Sec-Fetch-Mode",
        "sec-ch-ua",
        "Proxy-Authorization",
        "proxy-connection",
    });
    for (const auto name : names) {
        const auto message = check(name, "x");
        REQUIRE_MESSAGE(message.find(name) != std::string::npos, name);
    }
}

TEST_CASE("checkRequestHeaders fails a method override that names a forbidden method")
{
    REQUIRE(check("X-HTTP-Method", "TRACE") != "");
    REQUIRE(check("X-HTTP-Method-Override", "PUT, connect") != "");
    REQUIRE(check("X-Method-Override", " \"TRACK\" ") != "");
    REQUIRE(check("X-Method-Override", "DELETE, PATCH") == "");
}

TEST_CASE("checkRequestHeaders names the header without its value")
{
    const auto message = check("Cookie", "session=secret");
    REQUIRE(message.find("Cookie") != std::string::npos);
    REQUIRE(message.find("secret") == std::string::npos);
}
