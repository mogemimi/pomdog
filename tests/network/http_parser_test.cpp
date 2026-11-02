// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/http_parser.h"
#include "pomdog/network/http_response.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::u8;
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

} // namespace

TEST_CASE("HTTPParser reads header field names in any case")
{
    HTTPParser parser;

    SUBCASE("Content-Length")
    {
        std::string text = "HTTP/1.1 200 OK\r\n"
                           "content-type: application/json\r\n"
                           "content-length: 2\r\n"
                           "\r\n"
                           "{}";
        auto result = parser.parse(toBytes(text));
        REQUIRE(result.has_value());
        REQUIRE(*result == HTTPParseResult::EndOfFile);
        const auto response = parser.takeResponse();
        REQUIRE(response.contentLength == 2);
        REQUIRE(response.body.size() == 2);
    }
    SUBCASE("Transfer-Encoding")
    {
        std::string text = "HTTP/1.1 200 OK\r\n"
                           "TRANSFER-ENCODING: chunked\r\n"
                           "\r\n"
                           "2\r\n"
                           "{}\r\n"
                           "0\r\n"
                           "\r\n";
        auto result = parser.parse(toBytes(text));
        REQUIRE(result.has_value());
        REQUIRE(*result == HTTPParseResult::EndOfFile);
        const auto response = parser.takeResponse();
        REQUIRE(response.chunkedTransferEncoding);
        REQUIRE(toText(response.body) == "{}");
    }
}

TEST_CASE("HTTPParser reads a response split into packets")
{
    HTTPParser parser;

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

TEST_CASE("HTTPParser rejects an invalid status line")
{
    HTTPParser parser;

    std::string text = "HTTX/1.1 200 OK\r\n"
                       "\r\n";
    auto result = parser.parse(toBytes(text));
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() != nullptr);
}

TEST_CASE("HTTPParser waits for the empty line after the last chunk")
{
    HTTPParser parser;

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
