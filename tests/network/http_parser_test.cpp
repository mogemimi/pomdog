// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/http_method.h"
#include "pomdog/network/http_parser.h"
#include "pomdog/network/http_response.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <cstddef>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::u8;
using pomdog::network::HTTPMethod;
using pomdog::network::detail::HTTPParser;
using pomdog::network::detail::HTTPParseResult;

namespace {

[[nodiscard]] std::span<const u8>
toBytes(const std::string& text) noexcept
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    return std::span<const u8>{reinterpret_cast<const u8*>(text.data()), text.size()};
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

[[nodiscard]] std::string_view
toText(const std::vector<u8>& bytes) noexcept
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    return std::string_view{reinterpret_cast<const char*>(bytes.data()), bytes.size()};
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

/// Parses text as a whole response to a GET request without a size limit,
/// and requires it to complete.
[[nodiscard]] pomdog::network::HTTPResponse
parseWhole(const std::string& text)
{
    HTTPParser parser{HTTPMethod::Get, 0};
    auto result = parser.parse(toBytes(text));
    REQUIRE(result.has_value());
    REQUIRE(*result == HTTPParseResult::EndOfFile);
    return parser.takeResponse();
}

} // namespace

TEST_CASE("HTTPParser reads header field names in any case")
{
    SUBCASE("Content-Length")
    {
        const auto response = parseWhole(
            "HTTP/1.1 200 OK\r\n"
            "content-type: application/json\r\n"
            "content-length: 2\r\n"
            "\r\n"
            "{}");
        REQUIRE(response.contentLength == 2);
        REQUIRE(response.body.size() == 2);
    }
    SUBCASE("Transfer-Encoding")
    {
        const auto response = parseWhole(
            "HTTP/1.1 200 OK\r\n"
            "TRANSFER-ENCODING: chunked\r\n"
            "\r\n"
            "2\r\n"
            "{}\r\n"
            "0\r\n"
            "\r\n");
        REQUIRE(response.chunkedTransferEncoding);
        REQUIRE(toText(response.body) == "{}");
    }
}

TEST_CASE("HTTPParser reads a response split into packets")
{
    HTTPParser parser{HTTPMethod::Get, 0};

    SUBCASE("Content-Length")
    {
        const std::string header = "HTTP/1.1 200 OK\r\n"
                                   "Content-Length: 5\r\n"
                                   "\r\n"
                                   "he";
        const std::string rest = "llo";

        auto first = parser.parse(toBytes(header));
        REQUIRE(first.has_value());
        REQUIRE(*first == HTTPParseResult::WouldBlock);

        auto second = parser.parse(toBytes(rest));
        REQUIRE(second.has_value());
        REQUIRE(*second == HTTPParseResult::EndOfFile);

        const auto response = parser.takeResponse();
        REQUIRE(response.statusCode == 200);
        REQUIRE(response.status == "200 OK");
        REQUIRE(response.protocol == "HTTP/1.1");
        REQUIRE(response.headers.size() == 1);
        REQUIRE(toText(response.body) == "hello");
    }
    SUBCASE("Transfer-Encoding")
    {
        const std::string header = "HTTP/1.1 200 OK\r\n"
                                   "Transfer-Encoding: chunked\r\n"
                                   "\r\n"
                                   "3\r\n"
                                   "hel\r\n";
        const std::string rest = "2\r\n"
                                 "lo\r\n"
                                 "0\r\n"
                                 "\r\n";

        auto first = parser.parse(toBytes(header));
        REQUIRE(first.has_value());
        REQUIRE(*first == HTTPParseResult::WouldBlock);

        auto second = parser.parse(toBytes(rest));
        REQUIRE(second.has_value());
        REQUIRE(*second == HTTPParseResult::EndOfFile);

        const auto response = parser.takeResponse();
        REQUIRE(toText(response.body) == "hello");
    }
}

TEST_CASE("HTTPParser reads a response one byte at a time")
{
    const std::string text = "HTTP/1.1 200 OK\r\n"
                             "Transfer-Encoding: chunked\r\n"
                             "\r\n"
                             "3\r\n"
                             "hel\r\n"
                             "2;name=value\r\n"
                             "lo\r\n"
                             "0\r\n"
                             "Trailer-Field: ignored\r\n"
                             "\r\n";

    HTTPParser parser{HTTPMethod::Get, 0};
    for (std::size_t i = 0; i < text.size(); ++i) {
        auto result = parser.parse(toBytes(text).subspan(i, 1));
        REQUIRE(result.has_value());
        REQUIRE(*result == ((i + 1 == text.size()) ? HTTPParseResult::EndOfFile : HTTPParseResult::WouldBlock));
    }
    const auto response = parser.takeResponse();
    REQUIRE(toText(response.body) == "hello");
}

TEST_CASE("HTTPParser rejects an invalid status line")
{
    HTTPParser parser{HTTPMethod::Get, 0};

    std::string text = "HTTX/1.1 200 OK\r\n"
                       "\r\n";
    auto result = parser.parse(toBytes(text));
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() != nullptr);
}

TEST_CASE("HTTPParser waits for the empty line after the last chunk")
{
    HTTPParser parser{HTTPMethod::Get, 0};

    const std::string text = "HTTP/1.1 200 OK\r\n"
                             "Transfer-Encoding: chunked\r\n"
                             "\r\n"
                             "2\r\n"
                             "{}\r\n"
                             "0\r\n";
    const std::string end = "\r\n";

    auto first = parser.parse(toBytes(text));
    REQUIRE(first.has_value());
    REQUIRE(*first == HTTPParseResult::WouldBlock);

    auto second = parser.parse(toBytes(end));
    REQUIRE(second.has_value());
    REQUIRE(*second == HTTPParseResult::EndOfFile);

    const auto response = parser.takeResponse();
    REQUIRE(toText(response.body) == "{}");
}

TEST_CASE("HTTPParser reads many small chunks")
{
    std::string text = "HTTP/1.1 200 OK\r\n"
                       "Transfer-Encoding: chunked\r\n"
                       "\r\n";
    constexpr std::size_t count = 20000;
    for (std::size_t i = 0; i < count; ++i) {
        text += "1\r\n";
        text += static_cast<char>('a' + (i % 26));
        text += "\r\n";
    }
    text += "0\r\n\r\n";

    const auto response = parseWhole(text);
    REQUIRE(response.body.size() == count);
    REQUIRE(toText(response.body).substr(0, 3) == "abc");
}

TEST_CASE("HTTPParser prefers chunked encoding to Content-Length")
{
    const auto response = parseWhole(
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 100\r\n"
        "Transfer-Encoding: chunked\r\n"
        "\r\n"
        "5\r\n"
        "hello\r\n"
        "0\r\n"
        "\r\n");
    REQUIRE(response.chunkedTransferEncoding);
    REQUIRE(response.contentLength == 0);
    REQUIRE(toText(response.body) == "hello");
}

TEST_CASE("HTTPParser reads no body for HEAD, 204, and 304")
{
    SUBCASE("HEAD")
    {
        HTTPParser parser{HTTPMethod::Head, 0};
        auto result = parser.parse(toBytes("HTTP/1.1 200 OK\r\n"
                                           "Content-Length: 100\r\n"
                                           "\r\n"));
        REQUIRE(result.has_value());
        REQUIRE(*result == HTTPParseResult::EndOfFile);
        const auto response = parser.takeResponse();
        REQUIRE(response.contentLength == 100);
        REQUIRE(response.body.empty());
    }
    SUBCASE("204")
    {
        const auto response = parseWhole("HTTP/1.1 204 No Content\r\n\r\n");
        REQUIRE(response.statusCode == 204);
        REQUIRE(response.body.empty());
    }
    SUBCASE("304")
    {
        const auto response = parseWhole("HTTP/1.1 304 Not Modified\r\nContent-Length: 10\r\n\r\n");
        REQUIRE(response.statusCode == 304);
        REQUIRE(response.body.empty());
    }
}

TEST_CASE("HTTPParser skips informational responses")
{
    const auto response = parseWhole(
        "HTTP/1.1 100 Continue\r\n"
        "\r\n"
        "HTTP/1.1 103 Early Hints\r\n"
        "Link: </style.css>; rel=preload\r\n"
        "\r\n"
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 2\r\n"
        "\r\n"
        "ok");
    REQUIRE(response.statusCode == 200);
    REQUIRE(response.headers.size() == 1);
    REQUIRE(toText(response.body) == "ok");
}

TEST_CASE("HTTPParser reads a body without a length until the connection closes")
{
    HTTPParser parser{HTTPMethod::Get, 0};

    auto first = parser.parse(toBytes("HTTP/1.1 200 OK\r\n"
                                      "\r\n"
                                      "hello, "));
    REQUIRE(first.has_value());
    REQUIRE(*first == HTTPParseResult::WouldBlock);

    auto second = parser.parse(toBytes("world"));
    REQUIRE(second.has_value());
    REQUIRE(*second == HTTPParseResult::WouldBlock);

    auto finished = parser.finish();
    REQUIRE(finished.has_value());
    REQUIRE(*finished == HTTPParseResult::EndOfFile);
    REQUIRE(toText(parser.takeResponse().body) == "hello, world");
}

TEST_CASE("HTTPParser fails a response that the connection cuts short")
{
    SUBCASE("before the response")
    {
        HTTPParser parser{HTTPMethod::Get, 0};
        REQUIRE_FALSE(parser.finish().has_value());
    }
    SUBCASE("inside the body")
    {
        HTTPParser parser{HTTPMethod::Get, 0};
        auto result = parser.parse(toBytes("HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nhe"));
        REQUIRE(result.has_value());
        REQUIRE(*result == HTTPParseResult::WouldBlock);
        REQUIRE_FALSE(parser.finish().has_value());
    }
}

TEST_CASE("HTTPParser accepts header fields without a space after the colon")
{
    const auto response = parseWhole(
        "HTTP/1.1 200 OK\r\n"
        "Content-Length:2\r\n"
        "X-Tab:\ta\tb \r\n"
        "X-UTF8: \xE3\x81\x82\r\n"
        "\r\n"
        "ok");
    REQUIRE(response.contentLength == 2);
    REQUIRE(response.headers.size() == 3);
    REQUIRE(response.headers[1].second == "a\tb");
    REQUIRE(response.headers[2].second == "\xE3\x81\x82");
}

TEST_CASE("HTTPParser rejects invalid lengths and chunk sizes")
{
    SUBCASE("a chunk size that does not fit")
    {
        HTTPParser parser{HTTPMethod::Get, 0};
        auto result = parser.parse(toBytes("HTTP/1.1 200 OK\r\n"
                                           "Transfer-Encoding: chunked\r\n"
                                           "\r\n"
                                           "1ffffffffffffffff\r\n"));
        REQUIRE_FALSE(result.has_value());
    }
    SUBCASE("a chunk size that is not hexadecimal")
    {
        HTTPParser parser{HTTPMethod::Get, 0};
        auto result = parser.parse(toBytes("HTTP/1.1 200 OK\r\n"
                                           "Transfer-Encoding: chunked\r\n"
                                           "\r\n"
                                           "zz\r\n"));
        REQUIRE_FALSE(result.has_value());
    }
    SUBCASE("a negative Content-Length")
    {
        HTTPParser parser{HTTPMethod::Get, 0};
        auto result = parser.parse(toBytes("HTTP/1.1 200 OK\r\nContent-Length: -1\r\n\r\n"));
        REQUIRE_FALSE(result.has_value());
    }
    SUBCASE("two different Content-Length values")
    {
        HTTPParser parser{HTTPMethod::Get, 0};
        auto result = parser.parse(toBytes("HTTP/1.1 200 OK\r\nContent-Length: 2\r\nContent-Length: 3\r\n\r\nabc"));
        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("HTTPParser limits the size of the header")
{
    HTTPParser parser{HTTPMethod::Get, 0};
    auto result = parser.parse(toBytes("HTTP/1.1 200 OK\r\n"
                                       "X-Large: " +
                                       std::string(70000, 'a') + "\r\n"));
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("HTTPParser limits the size of the body")
{
    SUBCASE("Content-Length")
    {
        HTTPParser parser{HTTPMethod::Get, 4};
        auto result = parser.parse(toBytes("HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\n"));
        REQUIRE_FALSE(result.has_value());
    }
    SUBCASE("a body within the limit")
    {
        HTTPParser parser{HTTPMethod::Get, 5};
        auto result = parser.parse(toBytes("HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nhello"));
        REQUIRE(result.has_value());
        REQUIRE(*result == HTTPParseResult::EndOfFile);
    }
    SUBCASE("chunked")
    {
        HTTPParser parser{HTTPMethod::Get, 4};
        auto result = parser.parse(toBytes("HTTP/1.1 200 OK\r\n"
                                           "Transfer-Encoding: chunked\r\n"
                                           "\r\n"
                                           "3\r\n"
                                           "hel\r\n"
                                           "2\r\n"));
        REQUIRE_FALSE(result.has_value());
    }
    SUBCASE("until the connection closes")
    {
        HTTPParser parser{HTTPMethod::Get, 4};
        auto result = parser.parse(toBytes("HTTP/1.1 200 OK\r\n\r\nhello"));
        REQUIRE_FALSE(result.has_value());
    }
}
