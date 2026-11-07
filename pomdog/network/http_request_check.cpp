// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/http_request_check.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

/// ReservedHeader is a header that the client sets itself, and what to use
/// instead.
struct ReservedHeader final {
    std::string_view name;
    std::string_view reason;
};

constexpr auto reservedHeaders = std::to_array<ReservedHeader>({
    {.name = "Host", .reason = "the client sets it from HTTPRequest::url"},
    {.name = "Connection", .reason = "the client sets it from HTTPRequest::persistentConnection"},
    {.name = "Content-Length", .reason = "the client sets it from HTTPRequest::body"},
    {.name = "Transfer-Encoding", .reason = "the client sends a body with Content-Length"},
    {.name = "Accept-Encoding", .reason = "the client negotiates the content coding"},
    {.name = "Cookie", .reason = "the browser sends cookies by HTTPRequest::credentialsMode"},
});

// NOTE: The other forbidden request-headers of the Fetch standard, which
// browsers drop from fetch() without an error. Every platform fails them, so
// that a request does not work natively and in Node.js but not in a browser.
constexpr auto forbiddenHeaders = std::to_array<std::string_view>({
    "Accept-Charset",
    "Access-Control-Request-Headers",
    "Access-Control-Request-Method",
    "Cookie2",
    "Date",
    "DNT",
    "Expect",
    "Keep-Alive",
    "Origin",
    "Referer",
    "Set-Cookie",
    "TE",
    "Trailer",
    "Upgrade",
    "Via",
});

constexpr auto forbiddenPrefixes = std::to_array<std::string_view>({
    "Proxy-",
    "Sec-",
});

/// The headers that override the method, which are forbidden when they name a
/// forbidden method.
constexpr auto methodOverrideHeaders = std::to_array<std::string_view>({
    "X-HTTP-Method",
    "X-HTTP-Method-Override",
    "X-Method-Override",
});

constexpr auto forbiddenMethods = std::to_array<std::string_view>({
    "CONNECT",
    "TRACE",
    "TRACK",
});

[[nodiscard]] char
toLower(char c) noexcept
{
    if ((c >= 'A') && (c <= 'Z')) {
        return static_cast<char>((c - 'A') + 'a');
    }
    return c;
}

[[nodiscard]] bool
equalsIgnoringCase(std::string_view a, std::string_view b) noexcept
{
    return std::ranges::equal(a, b, [](char x, char y) { return toLower(x) == toLower(y); });
}

/// Returns true if name starts with prefix, ignoring case.
[[nodiscard]] bool
startsWithIgnoringCase(std::string_view name, std::string_view prefix) noexcept
{
    return (name.size() >= prefix.size()) && equalsIgnoringCase(name.substr(0, prefix.size()), prefix);
}

/// Returns true if value, a comma-separated list, names a forbidden method.
[[nodiscard]] bool
namesForbiddenMethod(std::string_view value) noexcept
{
    while (true) {
        const auto comma = value.find(',');
        auto method = value.substr(0, comma);
        while (!method.empty() && ((method.front() == ' ') || (method.front() == '\t') || (method.front() == '"'))) {
            method.remove_prefix(1);
        }
        while (!method.empty() && ((method.back() == ' ') || (method.back() == '\t') || (method.back() == '"'))) {
            method.remove_suffix(1);
        }
        if (std::ranges::any_of(forbiddenMethods, [&](std::string_view m) { return equalsIgnoringCase(method, m); })) {
            return true;
        }
        if (comma == std::string_view::npos) {
            return false;
        }
        value.remove_prefix(comma + 1);
    }
}

/// Returns true if browsers forbid fetch() to send the header.
[[nodiscard]] bool
isForbiddenHeader(std::string_view name, std::string_view value) noexcept
{
    const auto matches = [&](std::string_view forbidden) { return equalsIgnoringCase(name, forbidden); };
    if (std::ranges::any_of(forbiddenHeaders, matches)) {
        return true;
    }
    if (std::ranges::any_of(forbiddenPrefixes, [&](std::string_view prefix) { return startsWithIgnoringCase(name, prefix); })) {
        return true;
    }
    return std::ranges::any_of(methodOverrideHeaders, matches) && namesForbiddenMethod(value);
}

/// Returns true if c may appear in a header name, which is a token (RFC 9110,
/// section 5.6.2).
[[nodiscard]] bool
isTokenCharacter(char c) noexcept
{
    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9'))) {
        return true;
    }
    return std::string_view{"!#$%&'*+-.^_`|~"}.find(c) != std::string_view::npos;
}

} // namespace

std::expected<void, std::unique_ptr<Error>>
checkRequestMethod(HTTPMethod method)
{
    if ((method == HTTPMethod::Connect) || (method == HTTPMethod::Trace)) {
        const std::string_view name = (method == HTTPMethod::Connect) ? "CONNECT" : "TRACE";
        return std::unexpected(errors::make("the request must not use the method " + std::string{name} + ", because browsers do not let fetch() send it"));
    }
    return {};
}

std::expected<void, std::unique_ptr<Error>>
checkRequestHeaders(std::span<const std::pair<std::string, std::string>> headers)
{
    for (const auto& [name, value] : headers) {
        if (name.empty() || !std::ranges::all_of(name, isTokenCharacter)) {
            return std::unexpected(errors::make("the header name \"" + name + "\" is not a valid token"));
        }
        if (value.find_first_of(std::string_view{"\r\n\0", 3}) != std::string::npos) {
            return std::unexpected(errors::make("the header " + name + " has a line break or a null character"));
        }
        for (const auto& reserved : reservedHeaders) {
            if (equalsIgnoringCase(name, reserved.name)) {
                return std::unexpected(errors::make("the request must not set the header " + name + ", because " + std::string{reserved.reason}));
            }
        }
        if (isForbiddenHeader(name, value)) {
            return std::unexpected(errors::make("the request must not set the header " + name + ", because browsers do not let fetch() send it"));
        }
    }
    return {};
}

} // namespace pomdog::network::detail
