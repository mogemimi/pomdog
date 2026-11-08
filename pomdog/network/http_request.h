// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/http_method.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network {

/// HTTPRedirectMode tells what a client does when the server answers a request
/// with a redirect, a response with status 301, 302, 303, 307, or 308.
enum class HTTPRedirectMode : u8 {
    /// Fails the request at the redirect, without sending a request to the new
    /// location.
    Error,

    /// Follows up to 20 redirects as fetch() does, and returns the last
    /// response. A redirect without a Location header is the response. A 303,
    /// and a 301 or 302 to a POST request, becomes a GET request without the
    /// body and its Content-Type, Content-Encoding, Content-Language, and
    /// Content-Location headers; a 307 or 308 keeps the method and the body.
    /// Authorization is removed once the origin changes, but other headers,
    /// such as API keys, go to the new location. Native clients also fail a
    /// redirect from https to http.
    Follow,
};

/// HTTPCacheMode tells whether a request uses the HTTP cache of the client.
enum class HTTPCacheMode : u8 {
    /// Uses the cache as the client usually does. On the web, the browser
    /// caches responses as their headers allow, such as Cache-Control. Native
    /// clients have no cache, and send every request.
    Default,

    /// Neither reads nor writes the cache. Native clients have no cache, so it
    /// changes nothing there.
    NoStore,
};

/// HTTPCredentialsMode tells whether a request sends and stores cookies.
enum class HTTPCredentialsMode : u8 {
    /// Sends no cookies, and stores none from the response.
    Omit,

    /// Sends the cookies of the page's origin with requests to that origin,
    /// and stores those from their responses, as the browser does. The request
    /// fails on native platforms and in Node.js, which have no page origin.
    SameOrigin,
};

/// HTTPRequest represents an HTTP request that `HTTPClient::send()` sends.
struct HTTPRequest final {
    /// The method for the request.
    HTTPMethod method = HTTPMethod::Get;

    /// The URL for the request, such as "https://example.com/index.html".
    std::string url;

    /// The request headers as name and value pairs, sent in this order.
    ///
    /// The request fails on every platform when they hold a value with a line
    /// break, a header that the client sets itself (Host, Connection,
    /// Content-Length, Transfer-Encoding, and Accept-Encoding), or one that
    /// browsers do not let fetch() set, such as Cookie, Origin, Referer, and
    /// the names that start with Sec- or Proxy-. The error names the header,
    /// but not its value.
    std::vector<std::pair<std::string, std::string>> headers;

    /// The request body. The request fails on every platform when the method
    /// is GET or HEAD and the body is not empty, as fetch() does.
    std::vector<u8> body;

    /// True to keep the connection open after the response, so that a later
    /// request to the same server can reuse it. Ignored on the web, where the
    /// browser keeps connections.
    bool persistentConnection = true;

    /// What the client does when the server redirects the request. By
    /// default, a redirect fails the request.
    HTTPRedirectMode redirectMode = HTTPRedirectMode::Error;

    /// Whether the request uses the HTTP cache of the client.
    HTTPCacheMode cacheMode = HTTPCacheMode::Default;

    /// Whether the request sends and stores cookies. By default, it does not.
    HTTPCredentialsMode credentialsMode = HTTPCredentialsMode::Omit;
};

} // namespace pomdog::network
