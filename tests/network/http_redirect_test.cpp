// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/network/http_method.h"
#include "pomdog/network/http_redirect.h"
#include "pomdog/network/http_request.h"
#include "pomdog/network/http_response.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <algorithm>
#include <array>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using pomdog::network::HTTPMethod;
using pomdog::network::HTTPRequest;
using pomdog::network::HTTPResponse;
using pomdog::network::detail::getURLOrigin;
using pomdog::network::detail::isRedirectStatus;
using pomdog::network::detail::makeRedirectRequest;
using pomdog::network::detail::removeURLFragment;
using pomdog::network::detail::resolveURL;

namespace {

/// Returns reference resolved against base, or "error" when it fails.
[[nodiscard]] std::string
resolve(std::string_view base, std::string_view reference)
{
    auto result = resolveURL(base, reference);
    return result ? std::move(*result) : std::string{"error"};
}

[[nodiscard]] HTTPResponse
makeRedirect(int statusCode, std::string_view location)
{
    HTTPResponse response;
    response.statusCode = statusCode;
    response.headers.emplace_back("Location", location);
    return response;
}

[[nodiscard]] bool
hasHeader(const HTTPRequest& request, std::string_view name)
{
    return std::ranges::any_of(request.headers, [&](const auto& header) { return header.first == name; });
}

/// Returns a POST request with a body and the headers that describe it.
[[nodiscard]] HTTPRequest
makePost(std::string_view url)
{
    HTTPRequest request;
    request.method = HTTPMethod::Post;
    request.url = url;
    request.body = {1, 2, 3};
    request.headers = {{"Content-Type", "text/plain"}, {"X-API-Key", "key"}, {"Authorization", "Bearer token"}};
    return request;
}

} // namespace

TEST_CASE("isRedirectStatus accepts the redirect statuses of fetch()")
{
    for (const auto code : {301, 302, 303, 307, 308}) {
        REQUIRE(isRedirectStatus(code));
    }
    for (const auto code : {200, 300, 304, 305, 306, 309, 400}) {
        REQUIRE_FALSE(isRedirectStatus(code));
    }
}

TEST_CASE("removeURLFragment removes the part from #")
{
    REQUIRE(removeURLFragment("http://a/b?q#f") == "http://a/b?q");
    REQUIRE(removeURLFragment("http://a/b") == "http://a/b");
}

TEST_CASE("resolveURL resolves the normal examples of RFC 3986")
{
    constexpr std::string_view base = "http://a/b/c/d;p?q";
    constexpr auto examples = std::to_array<std::pair<std::string_view, std::string_view>>({
        {"g:h", "g:h"},
        {"g", "http://a/b/c/g"},
        {"./g", "http://a/b/c/g"},
        {"g/", "http://a/b/c/g/"},
        {"/g", "http://a/g"},
        {"//g", "http://g"},
        {"?y", "http://a/b/c/d;p?y"},
        {"g?y", "http://a/b/c/g?y"},
        {"#s", "http://a/b/c/d;p?q#s"},
        {"g#s", "http://a/b/c/g#s"},
        {"g?y#s", "http://a/b/c/g?y#s"},
        {";x", "http://a/b/c/;x"},
        {"g;x", "http://a/b/c/g;x"},
        {"g;x?y#s", "http://a/b/c/g;x?y#s"},
        {"", "http://a/b/c/d;p?q"},
        {".", "http://a/b/c/"},
        {"./", "http://a/b/c/"},
        {"..", "http://a/b/"},
        {"../", "http://a/b/"},
        {"../g", "http://a/b/g"},
        {"../..", "http://a/"},
        {"../../", "http://a/"},
        {"../../g", "http://a/g"},
    });
    for (const auto& [reference, expected] : examples) {
        REQUIRE_MESSAGE(resolve(base, reference) == expected, reference);
    }
}

TEST_CASE("resolveURL resolves the abnormal examples of RFC 3986")
{
    constexpr std::string_view base = "http://a/b/c/d;p?q";
    constexpr auto examples = std::to_array<std::pair<std::string_view, std::string_view>>({
        {"../../../g", "http://a/g"},
        {"../../../../g", "http://a/g"},
        {"/./g", "http://a/g"},
        {"/../g", "http://a/g"},
        {"g.", "http://a/b/c/g."},
        {".g", "http://a/b/c/.g"},
        {"g..", "http://a/b/c/g.."},
        {"..g", "http://a/b/c/..g"},
        {"./../g", "http://a/b/g"},
        {"./g/.", "http://a/b/c/g/"},
        {"g/./h", "http://a/b/c/g/h"},
        {"g/../h", "http://a/b/c/h"},
        {"g;x=1/./y", "http://a/b/c/g;x=1/y"},
        {"g;x=1/../y", "http://a/b/c/y"},
        {"g?y/./x", "http://a/b/c/g?y/./x"},
        {"g#s/../x", "http://a/b/c/g#s/../x"},
        {"http:g", "http:g"},
    });
    for (const auto& [reference, expected] : examples) {
        REQUIRE_MESSAGE(resolve(base, reference) == expected, reference);
    }
}

TEST_CASE("resolveURL lowercases the scheme and fails a space")
{
    REQUIRE(resolve("http://a/b", "HTTPS://Example.com/x") == "https://Example.com/x");
    REQUIRE(resolve("http://example.com", "path") == "http://example.com/path");
    REQUIRE(resolve("http://a/b", "/a b") == "error");
    REQUIRE(resolve("http://a/b", "/a\tb") == "error");
}

TEST_CASE("getURLOrigin adds the default port and lowercases the host")
{
    REQUIRE(getURLOrigin("https://Example.com/x?y") == "https://example.com:443");
    REQUIRE(getURLOrigin("HTTP://example.com") == "http://example.com:80");
    REQUIRE(getURLOrigin("http://example.com:8080/") == "http://example.com:8080");
    REQUIRE(getURLOrigin("http://user:password@example.com/") == "http://example.com:80");
    REQUIRE(getURLOrigin("http://[::1]:8080/") == "http://[::1]:8080");
}

TEST_CASE("makeRedirectRequest returns nothing without a Location header")
{
    HTTPRequest request;
    request.url = "http://example.com/a";
    HTTPResponse response;
    response.statusCode = 302;

    auto next = makeRedirectRequest(request, response);
    REQUIRE(next.has_value());
    REQUIRE_FALSE(next->has_value());
}

TEST_CASE("makeRedirectRequest resolves the location against the request URL")
{
    HTTPRequest request;
    request.url = "http://example.com/a/b?x";
    request.headers = {{"X-Test", "kept"}};

    auto next = makeRedirectRequest(request, makeRedirect(302, "c"));
    REQUIRE(next.has_value());
    REQUIRE(next->has_value());
    REQUIRE((*next)->url == "http://example.com/a/c");
    REQUIRE((*next)->method == HTTPMethod::Get);
    REQUIRE(hasHeader(**next, "X-Test"));
}

TEST_CASE("makeRedirectRequest changes the method to GET as fetch() does")
{
    for (const auto code : {301, 302, 303}) {
        auto next = makeRedirectRequest(makePost("http://example.com/a"), makeRedirect(code, "/b"));
        REQUIRE(next.has_value());
        REQUIRE(next->has_value());
        REQUIRE((*next)->method == HTTPMethod::Get);
        REQUIRE((*next)->body.empty());
        REQUIRE_FALSE(hasHeader(**next, "Content-Type"));
        REQUIRE(hasHeader(**next, "X-API-Key"));
        REQUIRE(hasHeader(**next, "Authorization"));
    }

    // NOTE: 301 and 302 change only POST, and 303 keeps HEAD.
    auto put = makePost("http://example.com/a");
    put.method = HTTPMethod::Put;
    auto next = makeRedirectRequest(put, makeRedirect(302, "/b"));
    REQUIRE(next.has_value());
    REQUIRE((*next)->method == HTTPMethod::Put);
    REQUIRE((*next)->body.size() == 3);

    HTTPRequest head;
    head.method = HTTPMethod::Head;
    head.url = "http://example.com/a";
    next = makeRedirectRequest(head, makeRedirect(303, "/b"));
    REQUIRE(next.has_value());
    REQUIRE((*next)->method == HTTPMethod::Head);
}

TEST_CASE("makeRedirectRequest keeps the method and the body for 307 and 308")
{
    for (const auto code : {307, 308}) {
        auto next = makeRedirectRequest(makePost("http://example.com/a"), makeRedirect(code, "/b"));
        REQUIRE(next.has_value());
        REQUIRE(next->has_value());
        REQUIRE((*next)->method == HTTPMethod::Post);
        REQUIRE((*next)->body.size() == 3);
        REQUIRE(hasHeader(**next, "Content-Type"));
    }
}

TEST_CASE("makeRedirectRequest removes Authorization when the origin changes")
{
    auto next = makeRedirectRequest(makePost("http://example.com/a"), makeRedirect(307, "http://example.com:8080/b"));
    REQUIRE(next.has_value());
    REQUIRE_FALSE(hasHeader(**next, "Authorization"));
    REQUIRE(hasHeader(**next, "X-API-Key"));

    next = makeRedirectRequest(makePost("http://example.com/a"), makeRedirect(307, "http://EXAMPLE.com:80/b"));
    REQUIRE(next.has_value());
    REQUIRE(hasHeader(**next, "Authorization"));
}

TEST_CASE("makeRedirectRequest fails a location that it cannot follow")
{
    HTTPRequest request;
    request.url = "https://example.com/a";

    REQUIRE_FALSE(makeRedirectRequest(request, makeRedirect(302, "http://example.com/b")).has_value());
    REQUIRE_FALSE(makeRedirectRequest(request, makeRedirect(302, "ftp://example.com/b")).has_value());
    REQUIRE_FALSE(makeRedirectRequest(request, makeRedirect(302, "/a b")).has_value());

    // NOTE: A redirect from http to https is allowed.
    request.url = "http://example.com/a";
    REQUIRE(makeRedirectRequest(request, makeRedirect(302, "https://example.com/b")).has_value());
}
