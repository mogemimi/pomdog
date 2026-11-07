// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/http_redirect.h"
#include "pomdog/network/http_method.h"
#include "pomdog/network/http_request.h"
#include "pomdog/network/http_response.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

/// The headers that describe a request body, which a redirect that changes
/// the method to GET removes (the request-body-header names of fetch()).
constexpr auto requestBodyHeaders = std::to_array<std::string_view>({
    "Content-Encoding",
    "Content-Language",
    "Content-Location",
    "Content-Type",
});

/// The components of a URL reference (RFC 3986, section 3). Each optional one
/// is absent when the reference does not have its delimiter.
struct URLParts final {
    std::optional<std::string_view> scheme;
    std::optional<std::string_view> authority;
    std::string_view path;
    std::optional<std::string_view> query;
    std::optional<std::string_view> fragment;
};

[[nodiscard]] char
toLowerASCII(char c) noexcept
{
    if ((c >= 'A') && (c <= 'Z')) {
        return static_cast<char>((c - 'A') + 'a');
    }
    return c;
}

[[nodiscard]] std::string
toLowerASCII(std::string_view s)
{
    std::string result{s};
    std::ranges::transform(result, std::begin(result), [](char c) { return toLowerASCII(c); });
    return result;
}

[[nodiscard]] bool
equalsIgnoringCase(std::string_view a, std::string_view b) noexcept
{
    return std::ranges::equal(a, b, [](char x, char y) { return toLowerASCII(x) == toLowerASCII(y); });
}

[[nodiscard]] bool
isAlpha(char c) noexcept
{
    return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

[[nodiscard]] bool
isSchemeCharacter(char c) noexcept
{
    return isAlpha(c) || ((c >= '0') && (c <= '9')) || (c == '+') || (c == '-') || (c == '.');
}

/// Splits reference into its components, as the regular expression of RFC
/// 3986, appendix B, does.
[[nodiscard]] URLParts
splitURL(std::string_view reference) noexcept
{
    URLParts parts;
    if (const auto hash = reference.find('#'); hash != std::string_view::npos) {
        parts.fragment = reference.substr(hash + 1);
        reference = reference.substr(0, hash);
    }
    if (const auto question = reference.find('?'); question != std::string_view::npos) {
        parts.query = reference.substr(question + 1);
        reference = reference.substr(0, question);
    }
    if (const auto colon = reference.find(':'); (colon != std::string_view::npos) && (colon > 0)) {
        const auto scheme = reference.substr(0, colon);
        if (isAlpha(scheme.front()) && std::ranges::all_of(scheme, isSchemeCharacter)) {
            parts.scheme = scheme;
            reference.remove_prefix(colon + 1);
        }
    }
    if (reference.starts_with("//")) {
        reference.remove_prefix(2);
        const auto slash = reference.find('/');
        parts.authority = reference.substr(0, slash);
        reference = (slash == std::string_view::npos) ? std::string_view{} : reference.substr(slash);
    }
    parts.path = reference;
    return parts;
}

/// Removes the last segment of output and its "/" (RFC 3986, section 5.2.4).
void removeLastSegment(std::string& output)
{
    const auto slash = output.rfind('/');
    output.erase((slash == std::string::npos) ? 0 : slash);
}

/// Removes the "." and ".." segments of path (RFC 3986, section 5.2.4).
[[nodiscard]] std::string
removeDotSegments(std::string_view input)
{
    std::string output;
    while (!input.empty()) {
        if (input.starts_with("../")) {
            input.remove_prefix(3);
        }
        else if (input.starts_with("./")) {
            input.remove_prefix(2);
        }
        else if (input.starts_with("/./")) {
            input.remove_prefix(2);
        }
        else if (input == "/.") {
            input = "/";
        }
        else if (input.starts_with("/../")) {
            input.remove_prefix(3);
            removeLastSegment(output);
        }
        else if (input == "/..") {
            input = "/";
            removeLastSegment(output);
        }
        else if ((input == ".") || (input == "..")) {
            input = {};
        }
        else {
            const auto end = input.find('/', input.starts_with('/') ? 1 : 0);
            const auto segment = input.substr(0, end);
            output += segment;
            input.remove_prefix(segment.size());
        }
    }
    return output;
}

/// Merges the path of a relative reference with the path of base (RFC 3986,
/// section 5.2.3).
[[nodiscard]] std::string
mergePaths(const URLParts& base, std::string_view path)
{
    if (base.authority.has_value() && base.path.empty()) {
        return "/" + std::string{path};
    }
    const auto slash = base.path.rfind('/');
    if (slash == std::string_view::npos) {
        return std::string{path};
    }
    return std::string{base.path.substr(0, slash + 1)} + std::string{path};
}

/// Returns the default port of an http or https scheme in lowercase.
[[nodiscard]] std::string_view
getDefaultPort(std::string_view scheme) noexcept
{
    return (scheme == "https") ? "443" : "80";
}

} // namespace

bool isRedirectStatus(i32 statusCode) noexcept
{
    return (statusCode == 301) || (statusCode == 302) || (statusCode == 303) || (statusCode == 307) || (statusCode == 308);
}

std::string_view
removeURLFragment(std::string_view url) noexcept
{
    return url.substr(0, url.find('#'));
}

std::expected<std::string, std::unique_ptr<Error>>
resolveURL(std::string_view base, std::string_view reference)
{
    if (std::ranges::any_of(reference, [](char c) { return (static_cast<unsigned char>(c) <= 0x20) || (c == 0x7f); })) {
        return std::unexpected(errors::make("the URL has a space or a control character"));
    }

    const auto b = splitURL(base);
    if (!b.scheme.has_value() || !b.authority.has_value()) {
        return std::unexpected(errors::make("the base URL is not absolute"));
    }
    const auto r = splitURL(reference);

    std::string_view scheme = *b.scheme;
    std::optional<std::string_view> authority = b.authority;
    std::string path;
    std::optional<std::string_view> query = r.query;

    if (r.scheme.has_value()) {
        scheme = *r.scheme;
        authority = r.authority;
        path = removeDotSegments(r.path);
    }
    else if (r.authority.has_value()) {
        authority = r.authority;
        path = removeDotSegments(r.path);
    }
    else if (r.path.empty()) {
        path = b.path;
        if (!r.query.has_value()) {
            query = b.query;
        }
    }
    else if (r.path.starts_with('/')) {
        path = removeDotSegments(r.path);
    }
    else {
        path = removeDotSegments(mergePaths(b, r.path));
    }

    // NOTE: Schemes are case-insensitive, and the client compares them in
    // lowercase.
    std::string result = toLowerASCII(scheme);
    result += ':';
    if (authority.has_value()) {
        result += "//";
        result += *authority;
    }
    result += path;
    if (query.has_value()) {
        result += '?';
        result += *query;
    }
    if (r.fragment.has_value()) {
        result += '#';
        result += *r.fragment;
    }
    return result;
}

std::string
getURLOrigin(std::string_view url)
{
    const auto parts = splitURL(url);
    const auto scheme = toLowerASCII(parts.scheme.value_or(std::string_view{}));
    auto authority = parts.authority.value_or(std::string_view{});

    // NOTE: Leave out user information, such as "user:password@".
    if (const auto at = authority.rfind('@'); at != std::string_view::npos) {
        authority.remove_prefix(at + 1);
    }

    // NOTE: An IPv6 address is in brackets, and has colons of its own.
    const auto hostEnd = authority.starts_with('[') ? authority.find(']') : std::string_view::npos;
    const auto colon = authority.find(':', (hostEnd == std::string_view::npos) ? 0 : hostEnd);
    const auto host = authority.substr(0, colon);
    auto port = (colon == std::string_view::npos) ? std::string_view{} : authority.substr(colon + 1);
    if (port.empty()) {
        port = getDefaultPort(scheme);
    }
    return scheme + "://" + toLowerASCII(host) + ":" + std::string{port};
}

std::expected<std::optional<HTTPRequest>, std::unique_ptr<Error>>
makeRedirectRequest(const HTTPRequest& request, const HTTPResponse& response)
{
    const auto location = std::ranges::find_if(response.headers, [](const auto& header) {
        return equalsIgnoringCase(header.first, "Location");
    });
    if (location == std::end(response.headers)) {
        return std::optional<HTTPRequest>{};
    }

    // NOTE: The errors leave out the location, which may hold a token.
    auto url = resolveURL(request.url, location->second);
    if (!url) {
        return std::unexpected(errors::wrap(std::move(url.error()), "the redirect location is not a valid URL"));
    }
    if (!url->starts_with("http://") && !url->starts_with("https://")) {
        return std::unexpected(errors::make("the redirect location is not an http or https URL"));
    }
    if (equalsIgnoringCase(splitURL(request.url).scheme.value_or(std::string_view{}), "https") && url->starts_with("http://")) {
        return std::unexpected(errors::make("refused a redirect from https to http"));
    }

    HTTPRequest next = request;
    next.url = std::move(*url);

    const bool isPost = (request.method == HTTPMethod::Post);
    const bool isGetOrHead = (request.method == HTTPMethod::Get) || (request.method == HTTPMethod::Head);
    const auto statusCode = response.statusCode;
    if ((((statusCode == 301) || (statusCode == 302)) && isPost) || ((statusCode == 303) && !isGetOrHead)) {
        next.method = HTTPMethod::Get;
        next.body.clear();
        std::erase_if(next.headers, [](const auto& header) {
            return std::ranges::any_of(requestBodyHeaders, [&](std::string_view name) { return equalsIgnoringCase(header.first, name); });
        });
    }

    // NOTE: Authorization is meant for the origin that the request named.
    if (getURLOrigin(request.url) != getURLOrigin(next.url)) {
        std::erase_if(next.headers, [](const auto& header) {
            return equalsIgnoringCase(header.first, "Authorization");
        });
    }
    return std::optional<HTTPRequest>{std::move(next)};
}

} // namespace pomdog::network::detail
