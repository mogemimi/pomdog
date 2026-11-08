// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/native/http_client_native.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/network/http_method.h"
#include "pomdog/network/http_parser.h"
#include "pomdog/network/http_redirect.h"
#include "pomdog/network/http_request_check.h"
#include "pomdog/network/tcp_stream.h"
#include "pomdog/network/tls_stream.h"
#include "pomdog/network/trust_store.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <chrono>
#include <span>
#include <sstream>
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

[[nodiscard]] std::string
createHeaderString(
    HTTPMethod method,
    const RequestURL& url,
    bool persistentConnection,
    const std::vector<std::pair<std::string, std::string>>& fields,
    std::optional<size_t> contentLength)
{
    std::ostringstream ss;

    ss << [&method]() -> const char* {
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
    }();

    ss << " " << url.resource << " HTTP/1.1\r\n";
    ss << "Host: " << url.host << "\r\n";

    if (persistentConnection) {
        ss << "Connection: "
           << "keep-alive"
           << "\r\n";
    }
    else {
        ss << "Connection: "
           << "close"
           << "\r\n";
    }

    if (contentLength.has_value()) {
        ss << "Content-Length: " << *contentLength << "\r\n";
    }

    // NOTE: The client decodes no content coding. Without the header, the
    // server could choose any (RFC 9110, section 12.5.3).
    ss << "Accept-Encoding: identity\r\n";

    for (const auto& field : fields) {
        ss << field.first << ": " << field.second << "\r\n";
    }

    ss << "\r\n";
    return ss.str();
}

/// The number of redirects that a request follows, as fetch() does.
constexpr int maxRedirects = 20;

[[nodiscard]] bool
equalsIgnoringCase(std::string_view a, std::string_view b) noexcept
{
    const auto toLower = [](char c) -> char {
        return ((c >= 'A') && (c <= 'Z')) ? static_cast<char>((c - 'A') + 'a') : c;
    };
    return std::ranges::equal(a, b, [&](char x, char y) { return toLower(x) == toLower(y); });
}

/// Removes the first element of list, a comma-separated list, and its comma,
/// and returns the element without the spaces around it.
[[nodiscard]] std::string_view
takeListElement(std::string_view& list) noexcept
{
    const auto comma = list.find(',');
    auto element = list.substr(0, comma);
    list = (comma == std::string_view::npos) ? std::string_view{} : list.substr(comma + 1);
    while (!element.empty() && ((element.front() == ' ') || (element.front() == '\t'))) {
        element.remove_prefix(1);
    }
    while (!element.empty() && ((element.back() == ' ') || (element.back() == '\t'))) {
        element.remove_suffix(1);
    }
    return element;
}

/// Returns why the client cannot pass the body of response, which has a
/// content coding that the client does not decode, or nothing.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
checkContentCoding(const HTTPResponse& response)
{
    if (response.body.empty()) {
        return {};
    }
    for (const auto& [name, value] : response.headers) {
        if (!equalsIgnoringCase(name, "Content-Encoding")) {
            continue;
        }
        std::string_view codings = value;
        while (!codings.empty()) {
            const auto coding = takeListElement(codings);
            if (!coding.empty() && !equalsIgnoringCase(coding, "identity")) {
                return std::unexpected(errors::make("the response has the content coding " + std::string{coding} + ", which the client does not decode"));
            }
        }
    }
    return {};
}

/// Returns true if the server keeps the connection of response open for
/// another request, which an HTTP/1.1 response without "Connection: close"
/// tells (RFC 9112, section 9.3). The client keeps no HTTP/1.0 connection.
[[nodiscard]] bool
canReuseConnection(const HTTPResponse& response)
{
    if (!equalsIgnoringCase(response.protocol, "HTTP/1.1")) {
        return false;
    }
    for (const auto& [name, value] : response.headers) {
        if (!equalsIgnoringCase(name, "Connection")) {
            continue;
        }
        std::string_view options = value;
        while (!options.empty()) {
            if (equalsIgnoringCase(takeListElement(options), "close")) {
                return false;
            }
        }
    }
    return true;
}

/// Returns true if method is idempotent (RFC 9110, section 9.2.2), so that a
/// request with it may go to the server twice.
[[nodiscard]] bool
isIdempotent(HTTPMethod method) noexcept
{
    return (method == HTTPMethod::Get) || (method == HTTPMethod::Head) || (method == HTTPMethod::Put) || (method == HTTPMethod::Delete);
}

} // namespace

struct HTTPClientNative::Session final {
    u64 requestID = 0;

    /// The request that the session sends now, which a redirect replaces.
    HTTPRequest request;

    /// The number of redirects that the session followed.
    int redirectCount = 0;

    /// The key of the connection, which an IdleConnection keeps.
    std::string connectionKey;

    std::shared_ptr<Stream> stream;

    /// The request header and body, written once the stream connects.
    std::vector<u8> requestBytes;

    /// Reads the response. send() creates it, since it needs the method.
    std::optional<HTTPParser> parser;

    /// Why the request failed before it started, reported by the next update().
    std::unique_ptr<Error> failure;

    bool persistentConnection = true;
    bool isRequestSent = false;
    bool isCompleted = false;

    /// True when the stream is a kept connection of an earlier request.
    bool isReusedConnection = false;

    /// True once a byte of the response arrived.
    bool isResponseStarted = false;

    /// When the request went out.
    std::chrono::steady_clock::time_point sentTime;
};

HTTPClientNative::HTTPClientNative(const HTTPClientOptions& options) noexcept
    : streamOptions_{.connectTimeout = options.connectTimeout, .idleTimeout = options.idleTimeout}
    , trustStore_(options.trustStore)
    , maxResponseSize_(options.maxResponseSize)
{
}

HTTPClientNative::~HTTPClientNative() = default;

u64 HTTPClientNative::send(HTTPRequest&& request)
{
    const auto requestID = nextRequestID_;
    ++nextRequestID_;

    auto session = std::make_unique<Session>();
    session->requestID = requestID;
    session->request = std::move(request);

    if (auto result = startSession(*session, session->request); !result) {
        session->failure = std::move(result.error());
    }
    sessions_.push_back(std::move(session));

    return requestID;
}

std::expected<void, std::unique_ptr<Error>>
HTTPClientNative::startSession(Session& session, const HTTPRequest& request)
{
    session.persistentConnection = request.persistentConnection;
    session.parser.emplace(request.method, maxResponseSize_);
    session.requestBytes.clear();
    session.isRequestSent = false;
    session.isReusedConnection = false;
    session.isResponseStarted = false;

    if (request.credentialsMode == HTTPCredentialsMode::SameOrigin) {
        return std::unexpected(errors::make("HTTPCredentialsMode::SameOrigin needs the origin of a web page, which native platforms do not have"));
    }

    // NOTE: A fragment names a part of the resource for the client, and is
    // not sent (RFC 9110, section 4.2.4).
    auto parsedURL = parseRequestURL(removeURLFragment(request.url), "http", "https");
    if (!parsedURL) {
        return std::unexpected(errors::wrap(std::move(parsedURL.error()), "invalid url " + request.url));
    }
    if (parsedURL->secure && (trustStore_ == nullptr)) {
        return std::unexpected(errors::make("https requests need HTTPClientOptions::trustStore"));
    }
    if (auto result = checkRequestMethod(request.method); !result) {
        return std::unexpected(std::move(result.error()));
    }
    if (auto result = checkRequestBody(request.method, request.body); !result) {
        return std::unexpected(std::move(result.error()));
    }
    if (auto result = checkRequestHeaders(request.headers); !result) {
        return std::unexpected(std::move(result.error()));
    }

    session.connectionKey = (parsedURL->secure ? "https://" : "http://") + parsedURL->address;

    // NOTE: A POST or a PUT tells its length even when it is empty, as fetch()
    // does (RFC 9110, section 8.6). Some servers answer 411 without it.
    std::optional<size_t> contentLength;
    if (!request.body.empty() || (request.method == HTTPMethod::Post) || (request.method == HTTPMethod::Put)) {
        contentLength = request.body.size();
    }

    const auto header = createHeaderString(
        request.method,
        *parsedURL,
        request.persistentConnection,
        request.headers,
        contentLength);

    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    static_assert(sizeof(decltype(header[0])) == sizeof(u8));
    const auto headerView = std::span<const u8>{
        reinterpret_cast<const u8*>(header.data()),
        header.size()};
    POMDOG_CLANG_UNSAFE_BUFFER_END

    session.requestBytes.reserve(headerView.size() + request.body.size());
    session.requestBytes.insert(std::end(session.requestBytes), std::begin(headerView), std::end(headerView));
    session.requestBytes.insert(std::end(session.requestBytes), std::begin(request.body), std::end(request.body));

    // NOTE: Reuse an open connection to the same server.
    auto idle = std::find_if(std::begin(idleConnections_), std::end(idleConnections_), [&](const IdleConnection& connection) {
        return (connection.key == session.connectionKey) && connection.stream->isConnected();
    });
    if (idle != std::end(idleConnections_)) {
        session.stream = std::move(idle->stream);
        session.isReusedConnection = true;
        idleConnections_.erase(idle);
        return {};
    }

    auto stream = parsedURL->secure
                      ? connectTLSStream(parsedURL->address, trustStore_, streamOptions_)
                      : connectTCPStream(parsedURL->address, streamOptions_);
    if (!stream) {
        return std::unexpected(errors::wrap(std::move(stream.error()), "failed to connect to server"));
    }
    session.stream = std::move(*stream);

    return {};
}

void HTTPClientNative::cancel(u64 requestID)
{
    auto session = std::find_if(std::begin(sessions_), std::end(sessions_), [&](const std::unique_ptr<Session>& s) {
        return s->requestID == requestID;
    });
    if (session != std::end(sessions_)) {
        auto& stream = (*session)->stream;
        if ((stream != nullptr) && !(*session)->isRequestSent && stream->isConnected()) {
            // NOTE: Nothing was sent on the connection, so a later request to
            // the same server can use it.
            idleConnections_.push_back(IdleConnection{
                .key = std::move((*session)->connectionKey),
                .stream = std::move(stream),
            });
        }
        else if (stream != nullptr) {
            // NOTE: The connection may still connect or hold half a response,
            // so it is closed rather than kept. Closing it does not wait.
            stream->close();
        }
        sessions_.erase(session);
    }

    std::erase_if(events_, [&](const HTTPEvent& event) {
        return event.requestID == requestID;
    });
}

void HTTPClientNative::update()
{
    // NOTE: Drop the idle connections that the server closed or that timed out.
    for (auto& connection : idleConnections_) {
        connection.stream->update();
        while (auto event = connection.stream->pollEvent()) {
            if (event->kind == StreamEventKind::Received) {
                // NOTE: Bytes on an idle connection belong to no request.
                connection.stream->close();
            }
        }
    }
    std::erase_if(idleConnections_, [](const IdleConnection& connection) {
        return !connection.stream->isConnected();
    });

    for (auto& session : sessions_) {
        updateSession(*session);
    }
    std::erase_if(sessions_, [](const std::unique_ptr<Session>& session) {
        return session->isCompleted;
    });
}

void HTTPClientNative::updateSession(Session& session)
{
    if (session.failure != nullptr) {
        complete(session, std::unexpected(std::move(session.failure)));
        return;
    }

    POMDOG_ASSERT(session.stream != nullptr);
    auto& stream = *session.stream;
    stream.update();

    POMDOG_ASSERT(session.parser.has_value());
    auto& parser = *session.parser;

    while (auto event = stream.pollEvent()) {
        switch (event->kind) {
        case StreamEventKind::Connected:
            break;
        case StreamEventKind::Received: {
            session.isResponseStarted = true;
            auto result = parser.parse(event->data);
            if (!result) {
                stream.close();
                complete(session, std::unexpected(std::move(result.error())));
                return;
            }
            if (*result == HTTPParseResult::EndOfFile) {
                auto response = parser.takeResponse();
                if (session.persistentConnection && stream.isConnected() && canReuseConnection(response)) {
                    idleConnections_.push_back(IdleConnection{
                        .key = std::move(session.connectionKey),
                        .stream = std::move(session.stream),
                    });
                }
                else {
                    stream.close();
                }
                finishResponse(session, std::move(response));
                return;
            }
            break;
        }
        case StreamEventKind::Disconnected: {
            // NOTE: A server may close a kept connection at any time, even as
            // a request arrives on it (RFC 9112, section 9.3.1). The request
            // goes again on a new connection when it never went out, or when
            // its method is idempotent, so that the server may get it twice.
            // A connection that ends after idleTimeout may have timed out,
            // and a server that answers nothing gets no second request. The
            // other kept connections to the server may be closed too, so
            // they are dropped.
            const auto idleTimeout = streamOptions_.idleTimeout;
            const bool mayBeIdleTimeout = (idleTimeout > Duration::zero()) && ((std::chrono::steady_clock::now() - session.sentTime) >= idleTimeout);
            if (session.isReusedConnection && !session.isResponseStarted &&
                (!session.isRequestSent || (isIdempotent(session.request.method) && !mayBeIdleTimeout))) {
                std::erase_if(idleConnections_, [&](const IdleConnection& connection) {
                    return connection.key == session.connectionKey;
                });
                session.stream = nullptr;
                if (auto result = startSession(session, session.request); !result) {
                    complete(session, std::unexpected(std::move(result.error())));
                }
                return;
            }
            if (event->error != nullptr) {
                complete(session, std::unexpected(errors::wrap(std::move(event->error), "HTTP request error")));
                return;
            }
            // NOTE: The server closed the connection, which ends a body that
            // has no length.
            if (auto result = parser.finish(); !result) {
                complete(session, std::unexpected(errors::wrap(std::move(result.error()), "HTTP request error")));
                return;
            }
            finishResponse(session, parser.takeResponse());
            return;
        }
        }
    }

    if (!session.isRequestSent && stream.isConnected()) {
        session.isRequestSent = true;
        session.sentTime = std::chrono::steady_clock::now();
        if (auto result = stream.write(session.requestBytes); !result) {
            stream.close();
            complete(session, std::unexpected(std::move(result.error())));
            return;
        }
    }
}

void HTTPClientNative::finishResponse(Session& session, HTTPResponse&& response)
{
    response.url = removeURLFragment(session.request.url);

    if (!isRedirectStatus(response.statusCode)) {
        if (auto result = checkContentCoding(response); !result) {
            complete(session, std::unexpected(std::move(result.error())));
            return;
        }
        complete(session, std::move(response));
        return;
    }

    if (session.request.redirectMode == HTTPRedirectMode::Error) {
        complete(session, std::unexpected(errors::make("the server redirected the request with status " + std::to_string(response.statusCode) + ", and redirectMode is Error")));
        return;
    }

    auto next = makeRedirectRequest(session.request, response);
    if (!next) {
        complete(session, std::unexpected(std::move(next.error())));
        return;
    }
    if (!next->has_value()) {
        // NOTE: A redirect without a location is the response, as in fetch().
        complete(session, std::move(response));
        return;
    }
    if (session.redirectCount >= maxRedirects) {
        complete(session, std::unexpected(errors::make("the request was redirected more than " + std::to_string(maxRedirects) + " times")));
        return;
    }

    ++session.redirectCount;
    session.request = std::move(**next);
    session.stream = nullptr;
    if (auto result = startSession(session, session.request); !result) {
        complete(session, std::unexpected(std::move(result.error())));
    }
}

void HTTPClientNative::complete(Session& session, std::expected<HTTPResponse, std::unique_ptr<Error>>&& result)
{
    POMDOG_ASSERT(!session.isCompleted);
    events_.push_back(HTTPEvent{.requestID = session.requestID, .result = std::move(result)});
    session.stream = nullptr;
    session.isCompleted = true;
}

std::optional<HTTPEvent>
HTTPClientNative::pollEvent()
{
    if (events_.empty()) {
        return std::nullopt;
    }
    auto event = std::move(events_.front());
    events_.pop_front();
    return event;
}

} // namespace pomdog::network::detail
