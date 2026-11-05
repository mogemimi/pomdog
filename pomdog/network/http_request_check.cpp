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
    }
    return {};
}

} // namespace pomdog::network::detail
