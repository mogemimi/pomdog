// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/network/web_socket_check.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using pomdog::network::detail::checkWebSocketClose;
using pomdog::network::detail::checkWebSocketProtocols;
using pomdog::network::detail::isValidUTF8;
using pomdog::network::detail::parseWebSocketURL;

TEST_CASE("parseWebSocketURL splits ws and wss URLs")
{
    auto url = parseWebSocketURL("ws://example.com/chat?room=1");
    REQUIRE(url.has_value());
    REQUIRE_FALSE(url->secure);
    REQUIRE(url->address == "example.com:80");
    REQUIRE(url->host == "example.com");
    REQUIRE(url->resource == "/chat?room=1");

    url = parseWebSocketURL("WSS://example.com:8443");
    REQUIRE(url.has_value());
    REQUIRE(url->secure);
    REQUIRE(url->address == "example.com:8443");
    REQUIRE(url->host == "example.com:8443");
    REQUIRE(url->resource == "/");

    url = parseWebSocketURL("wss://example.com?token=abc");
    REQUIRE(url.has_value());
    REQUIRE(url->address == "example.com:443");
    REQUIRE(url->resource == "/?token=abc");

    url = parseWebSocketURL("ws://[::1]:8080/socket");
    REQUIRE(url.has_value());
    REQUIRE(url->address == "[::1]:8080");
    REQUIRE(url->host == "[::1]:8080");
    REQUIRE(url->resource == "/socket");

    url = parseWebSocketURL("ws://[::1]");
    REQUIRE(url.has_value());
    REQUIRE(url->address == "[::1]:80");
    REQUIRE(url->host == "[::1]");
}

TEST_CASE("parseWebSocketURL fails URLs that browsers fail")
{
    for (const auto url : std::to_array<std::string_view>({
             "http://example.com/",
             "example.com/chat",
             "ws://example.com/#fragment",
             "ws://user:password@example.com/",
             "ws:///path",
             "ws://:80/",
             "ws://[::1/",
             "ws://example.com:http/",
             "ws://example.com:65536/",
             "ws://example.com/a b",
             "ws://example.com/\x7f",
         })) {
        CAPTURE(url);
        REQUIRE_FALSE(parseWebSocketURL(url).has_value());
    }
}

TEST_CASE("checkWebSocketProtocols fails invalid and repeated subprotocols")
{
    REQUIRE(checkWebSocketProtocols(std::vector<std::string>{}).has_value());
    REQUIRE(checkWebSocketProtocols(std::vector<std::string>{"chat", "v2.chat", "Chat"}).has_value());

    REQUIRE_FALSE(checkWebSocketProtocols(std::vector<std::string>{""}).has_value());
    REQUIRE_FALSE(checkWebSocketProtocols(std::vector<std::string>{"two words"}).has_value());
    REQUIRE_FALSE(checkWebSocketProtocols(std::vector<std::string>{"a,b"}).has_value());
    REQUIRE_FALSE(checkWebSocketProtocols(std::vector<std::string>{"chat", "chat"}).has_value());
}

TEST_CASE("checkWebSocketClose takes the codes and reasons that browsers take")
{
    REQUIRE(checkWebSocketClose(1000, "").has_value());
    REQUIRE(checkWebSocketClose(3000, "bye").has_value());
    REQUIRE(checkWebSocketClose(4999, std::string(123, 'r')).has_value());

    for (const auto code : std::to_array<pomdog::u16>({0, 999, 1001, 1002, 1005, 1011, 2999, 5000})) {
        CAPTURE(code);
        REQUIRE_FALSE(checkWebSocketClose(code, "").has_value());
    }
    REQUIRE_FALSE(checkWebSocketClose(1000, std::string(124, 'r')).has_value());
    REQUIRE_FALSE(checkWebSocketClose(1000, "\xff").has_value());
}

TEST_CASE("isValidUTF8 fails overlong forms, surrogates, and truncated sequences")
{
    REQUIRE(isValidUTF8(""));
    REQUIRE(isValidUTF8("hello"));
    REQUIRE(isValidUTF8("\xe3\x81\x82"));
    REQUIRE(isValidUTF8("\xf0\x9f\x90\xb6"));
    REQUIRE(isValidUTF8(std::string_view{"a\0b", 3}));

    REQUIRE_FALSE(isValidUTF8("\xc0\xaf"));
    REQUIRE_FALSE(isValidUTF8("\xed\xa0\x80"));
    REQUIRE_FALSE(isValidUTF8("\xf4\x90\x80\x80"));
    REQUIRE_FALSE(isValidUTF8("\xe3\x81"));
    REQUIRE_FALSE(isValidUTF8("\xff"));
}
