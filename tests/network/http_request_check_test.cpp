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

using pomdog::network::detail::checkRequestHeaders;

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
