// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/http_parser.h"
#include "pomdog/network/http_response.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <span>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::u8;
using pomdog::detail::HTTPParser;
using pomdog::detail::HTTPParseResult;

namespace {

[[nodiscard]] std::span<u8>
toBytes(std::string& text) noexcept
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    return std::span<u8>{reinterpret_cast<u8*>(text.data()), text.size()};
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
        auto [result, err] = parser.parse(toBytes(text));
        REQUIRE(err == nullptr);
        REQUIRE(result == HTTPParseResult::EndOfFile);
        const auto response = parser.getResponse();
        REQUIRE(response != nullptr);
        REQUIRE(response->contentLength == 2);
        REQUIRE(response->body.size() == 2);
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
        auto [result, err] = parser.parse(toBytes(text));
        REQUIRE(err == nullptr);
        REQUIRE(result == HTTPParseResult::EndOfFile);
        const auto response = parser.getResponse();
        REQUIRE(response != nullptr);
        REQUIRE(response->chunkedTransferEncoding);
    }
}
