// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/emscripten/http_client_emscripten.h"
#include "pomdog/network/http_method.h"
#include "pomdog/network/http_redirect.h"
#include "pomdog/network/http_request.h"
#include "pomdog/network/http_request_check.h"
#include "pomdog/network/http_response.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten.h>

#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

// NOTE: The JavaScript below is part of a C macro. It uses double quotes for
// strings and no comments, because the preprocessor reads it as C tokens, and
// clang-format must not touch it. Request IDs pass as doubles, which hold
// integers up to 2^53 exactly. Headers pass as "name: value" lines. An event
// with status 0 is a failure, since a response to fetch() has a status from
// 200 to 599.
//
// EM_JS defines variables that only the linker reads, without declarations.
POMDOG_EMCC_SUPPRESS_WARNING_PUSH
POMDOG_EMCC_SUPPRESS_WARNING("-Wmissing-variable-declarations")

// clang-format off
EM_JS_DEPS(pomdog_http, "$UTF8ToString,$stringToUTF8,$lengthBytesUTF8");

EM_JS(int, pomdog_http_create, (), {
    if (!Module.pomdogHTTP) {
        Module.pomdogHTTP = {nextClientID: 1, clients: new Map()};
    }
    var http = Module.pomdogHTTP;
    var clientID = http.nextClientID++;
    http.clients.set(clientID, {queue: [], requests: new Map()});
    return clientID;
});

EM_JS(void, pomdog_http_destroy, (int clientID), {
    var http = Module.pomdogHTTP;
    var client = http.clients.get(clientID);
    if (!client) {
        return;
    }
    http.clients.delete(clientID);
    client.requests.forEach(function(request) {
        clearTimeout(request.timer);
        request.controller.abort();
    });
    client.requests.clear();
});

EM_JS(int, pomdog_http_has_page_origin, (), {
    return (typeof location !== "undefined") ? 1 : 0;
});

EM_JS(void, pomdog_http_send, (int clientID, double requestID, const char* method, const char* url, const char* headerLines, const unsigned char* body, int bodySize, double idleTimeout, double maxBodySize, int followRedirects, int noStore, int sameOrigin), {
    var client = Module.pomdogHTTP.clients.get(clientID);
    var request = {controller: new AbortController(), timer: 0, timedOut: false, tooLarge: false};
    client.requests.set(requestID, request);

    var finish = function(e) {
        clearTimeout(request.timer);
        if (client.requests.get(requestID) !== request) {
            return;
        }
        client.requests.delete(requestID);
        e.requestID = requestID;
        client.queue.push(e);
    };
    var touch = function() {
        if (idleTimeout <= 0) {
            return;
        }
        clearTimeout(request.timer);
        request.timer = setTimeout(function() {
            request.timedOut = true;
            request.controller.abort();
        }, idleTimeout);
    };

    var headers = [];
    UTF8ToString(headerLines).split("\n").forEach(function(line) {
        var colon = line.indexOf(":");
        if (colon > 0) {
            headers.push([line.substring(0, colon), line.substring(colon + 1).trim()]);
        }
    });
    var init = {
        method: UTF8ToString(method),
        headers: headers,
        signal: request.controller.signal,
        redirect: followRedirects ? "follow" : "error",
        cache: noStore ? "no-store" : "default",
        credentials: sameOrigin ? "same-origin" : "omit"
    };
    if (bodySize > 0) {
        init.body = HEAPU8.slice(body, body + bodySize);
    }

    var response = null;
    var chunks = [];
    var size = 0;
    var readBody = function(reader) {
        return reader.read().then(function(result) {
            if (result.done) {
                return undefined;
            }
            touch();
            chunks.push(result.value);
            size += result.value.length;
            if ((maxBodySize > 0) && (size > maxBodySize)) {
                request.tooLarge = true;
                request.controller.abort();
                throw new Error("the response body is larger than " + String(maxBodySize) + " bytes");
            }
            return readBody(reader);
        });
    };

    touch();
    fetch(UTF8ToString(url), init).then(function(r) {
        response = r;
        touch();
        return r.body ? readBody(r.body.getReader()) : undefined;
    }).then(function() {
        var data = new Uint8Array(size);
        var offset = 0;
        chunks.forEach(function(chunk) {
            data.set(chunk, offset);
            offset += chunk.length;
        });
        var lines = "";
        response.headers.forEach(function(value, name) {
            lines += name + ": " + value + "\n";
        });
        finish({status: response.status, statusText: response.statusText, headers: lines, body: data, url: response.url, error: ""});
    }).catch(function(e) {
        var message = request.timedOut ? "timeout socket connection" : String((e && e.message) || e);
        if (request.tooLarge) {
            message = "the response body is larger than " + String(maxBodySize) + " bytes";
        }
        else if (!request.timedOut && e && e.cause) {
            message += ", " + String(e.cause.message || e.cause);
        }
        finish({status: 0, statusText: "", headers: "", body: new Uint8Array(0), url: "", error: message});
    });
});

EM_JS(void, pomdog_http_cancel, (int clientID, double requestID), {
    var client = Module.pomdogHTTP.clients.get(clientID);
    var request = client.requests.get(requestID);
    if (request) {
        client.requests.delete(requestID);
        clearTimeout(request.timer);
        request.controller.abort();
    }
    client.queue = client.queue.filter(function(e) {
        return e.requestID !== requestID;
    });
});

EM_JS(int, pomdog_http_peek, (int clientID, double* requestID, int* fields), {
    var client = Module.pomdogHTTP.clients.get(clientID);
    if (!client || (client.queue.length === 0)) {
        return 0;
    }
    var e = client.queue[0];
    HEAPF64[requestID >> 3] = e.requestID;
    var index = fields >> 2;
    HEAP32[index] = e.status;
    HEAP32[index + 1] = lengthBytesUTF8(e.statusText);
    HEAP32[index + 2] = lengthBytesUTF8(e.headers);
    HEAP32[index + 3] = e.body.length;
    HEAP32[index + 4] = lengthBytesUTF8(e.error);
    HEAP32[index + 5] = lengthBytesUTF8(e.url);
    return 1;
});

EM_JS(void, pomdog_http_pop, (int clientID, char* statusText, char* headerLines, unsigned char* body, char* error, char* url), {
    var e = Module.pomdogHTTP.clients.get(clientID).queue.shift();
    var write = function(text, buffer) {
        var size = lengthBytesUTF8(text);
        if (size > 0) {
            stringToUTF8(text, buffer, size + 1);
        }
    };
    write(e.statusText, statusText);
    write(e.headers, headerLines);
    write(e.error, error);
    write(e.url, url);
    if (e.body.length > 0) {
        HEAPU8.set(e.body, body);
    }
});
// clang-format on

POMDOG_EMCC_SUPPRESS_WARNING_POP

namespace pomdog::network::detail {
namespace {

[[nodiscard]] const char*
toMethodString(HTTPMethod method) noexcept
{
    switch (method) {
    case HTTPMethod::Get:
        return "GET";
    case HTTPMethod::Head:
        return "HEAD";
    case HTTPMethod::Post:
        return "POST";
    case HTTPMethod::Put:
        return "PUT";
    case HTTPMethod::Delete:
        return "DELETE";
    case HTTPMethod::Connect:
        return "CONNECT";
    case HTTPMethod::Trace:
        return "TRACE";
    }
    return "UNKNOWN";
}

/// Returns why request cannot start, or nothing when it can.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
validateRequest(const HTTPRequest& request, bool hasTrustStore)
{
    // NOTE: The client fails the same URLs as the native one, such as one with
    // a space, which the browser would percent-encode.
    auto url = parseRequestURL(removeURLFragment(request.url), "http", "https");
    if (!url) {
        return std::unexpected(errors::wrap(std::move(url.error()), "invalid url " + request.url));
    }
    if (url->secure && !hasTrustStore) {
        return std::unexpected(errors::make("https requests need HTTPClientOptions::trustStore"));
    }
    if ((request.credentialsMode == HTTPCredentialsMode::SameOrigin) && (pomdog_http_has_page_origin() == 0)) {
        // NOTE: Node.js has no page, and so no origin for the cookies.
        return std::unexpected(errors::make("HTTPCredentialsMode::SameOrigin needs the origin of a web page, which this environment does not have"));
    }
    if (auto result = checkRequestMethod(request.method); !result) {
        return std::unexpected(std::move(result.error()));
    }
    if (auto result = checkRequestBody(request.method, request.body); !result) {
        return std::unexpected(std::move(result.error()));
    }

    // NOTE: The headers pass to JavaScript as lines, which the check keeps
    // free of line breaks.
    if (auto result = checkRequestHeaders(request.headers); !result) {
        return std::unexpected(std::move(result.error()));
    }
    if (request.body.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        return std::unexpected(errors::make("the request body is too large"));
    }
    return {};
}

/// Returns the response that JavaScript described with the status code, the
/// status text, and the header lines.
[[nodiscard]] HTTPResponse
makeResponse(i32 statusCode, std::string_view statusText, std::string_view headerLines, std::vector<u8>&& body, std::string&& url)
{
    HTTPResponse response;
    response.url = std::move(url);
    response.statusCode = statusCode;
    response.status = std::to_string(statusCode);
    if (!statusText.empty()) {
        response.status += ' ';
        response.status += statusText;
    }

    // NOTE: fetch() names each header in lowercase, and does not tell the
    // HTTP version, so protocol stays empty.
    while (!headerLines.empty()) {
        const auto end = headerLines.find('\n');
        const auto line = headerLines.substr(0, end);
        headerLines = (end == std::string_view::npos) ? std::string_view{} : headerLines.substr(end + 1);

        const auto separator = line.find(": ");
        if (separator == std::string_view::npos) {
            continue;
        }
        const auto name = line.substr(0, separator);
        const auto value = line.substr(separator + 2);
        if (name == "content-length") {
            std::size_t length = 0;
            if (auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), length); ec == std::errc{}) {
                response.contentLength = length;
            }
        }
        else if (name == "transfer-encoding") {
            response.chunkedTransferEncoding = (value.find("chunked") != std::string_view::npos);
        }
        response.headers.emplace_back(name, value);
    }

    response.body = std::move(body);
    return response;
}

} // namespace

HTTPClientEmscripten::HTTPClientEmscripten(const HTTPClientOptions& options) noexcept
    : trustStore_(options.trustStore)
    , idleTimeoutMilliseconds_(std::chrono::duration_cast<std::chrono::duration<f64, std::milli>>(options.idleTimeout).count())
    , maxResponseSize_(static_cast<f64>(options.maxResponseSize))
    , clientID_(pomdog_http_create())
{
}

HTTPClientEmscripten::~HTTPClientEmscripten()
{
    pomdog_http_destroy(clientID_);
}

u64 HTTPClientEmscripten::send(HTTPRequest&& request)
{
    const auto requestID = nextRequestID_;
    ++nextRequestID_;

    if (auto result = validateRequest(request, trustStore_ != nullptr); !result) {
        failures_.push_back(HTTPEvent{.requestID = requestID, .result = std::unexpected(std::move(result.error()))});
        return requestID;
    }

    std::string headerLines;
    for (const auto& [name, value] : request.headers) {
        headerLines += name;
        headerLines += ": ";
        headerLines += value;
        headerLines += '\n';
    }

    // NOTE: The browser keeps connections, so persistentConnection has no
    // effect.
    pomdog_http_send(
        clientID_,
        static_cast<double>(requestID),
        toMethodString(request.method),
        request.url.c_str(),
        headerLines.c_str(),
        request.body.data(),
        static_cast<int>(request.body.size()),
        idleTimeoutMilliseconds_,
        maxResponseSize_,
        (request.redirectMode == HTTPRedirectMode::Follow) ? 1 : 0,
        (request.cacheMode == HTTPCacheMode::NoStore) ? 1 : 0,
        (request.credentialsMode == HTTPCredentialsMode::SameOrigin) ? 1 : 0);

    return requestID;
}

void HTTPClientEmscripten::cancel(u64 requestID)
{
    pomdog_http_cancel(clientID_, static_cast<double>(requestID));

    const auto matches = [&](const HTTPEvent& event) {
        return event.requestID == requestID;
    };
    std::erase_if(failures_, matches);
    std::erase_if(events_, matches);
}

void HTTPClientEmscripten::update()
{
    for (auto& failure : failures_) {
        events_.push_back(std::move(failure));
    }
    failures_.clear();

    std::array<int, 6> fields = {};
    double requestID = 0.0;
    while (pomdog_http_peek(clientID_, &requestID, fields.data()) != 0) {
        std::string statusText(static_cast<std::size_t>(fields[1]), '\0');
        std::string headerLines(static_cast<std::size_t>(fields[2]), '\0');
        std::vector<u8> body(static_cast<std::size_t>(fields[3]));
        std::string error(static_cast<std::size_t>(fields[4]), '\0');
        std::string url(static_cast<std::size_t>(fields[5]), '\0');

        // NOTE: The browser writes a null terminator after each text, into the
        // byte that std::string keeps past its end.
        pomdog_http_pop(clientID_, statusText.data(), headerLines.data(), body.data(), error.data(), url.data());

        HTTPEvent event{.requestID = static_cast<u64>(requestID), .result = HTTPResponse{}};
        if (fields[0] == 0) {
            event.result = std::unexpected(errors::make("HTTP request error: " + error));
        }
        else {
            event.result = makeResponse(fields[0], statusText, headerLines, std::move(body), std::move(url));
        }
        events_.push_back(std::move(event));
    }
}

std::optional<HTTPEvent>
HTTPClientEmscripten::pollEvent()
{
    if (events_.empty()) {
        return std::nullopt;
    }
    auto event = std::move(events_.front());
    events_.pop_front();
    return event;
}

} // namespace pomdog::network::detail
