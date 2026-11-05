// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/native/http_client_native.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/network/http_method.h"
#include "pomdog/network/http_parser.h"
#include "pomdog/network/tcp_stream.h"
#include "pomdog/network/tls_stream.h"
#include "pomdog/network/trust_store.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <span>
#include <sstream>
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

struct URLParseResult final {
    std::string_view protocolScheme;
    std::string_view hostName;
    std::string_view port;
    std::string_view path;
};

[[nodiscard]] std::expected<URLParseResult, std::unique_ptr<Error>>
parseURL(std::string_view url)
{
    std::string_view source = url;
    URLParseResult result;

    if (auto iter = source.find("://"); iter == std::string_view::npos) {
        return std::unexpected(errors::make("no protocol scheme (i.e. 'https://' or 'http://') in request URL"));
    }
    else {
        result.protocolScheme = url.substr(0, iter + 3);
        source.remove_prefix(result.protocolScheme.size());
    }

    if (auto iter = source.find("/"); iter != std::string_view::npos) {
        result.hostName = source.substr(0, iter);
        result.path = source.substr(iter);
    }
    else {
        result.hostName = source;
    }

    if (result.hostName.empty()) {
        return std::unexpected(errors::make("no Host in request URL"));
    }

    source = result.hostName;

    if (auto iter = source.find(":"); iter != std::string_view::npos) {
        result.hostName = source.substr(0, iter);
        result.port = source.substr(iter);

        if (result.port.size() <= 1) {
            return std::unexpected(errors::make("no Port in request URL"));
        }
        result.port.remove_prefix(1);
    }

    return result;
}

[[nodiscard]] std::string
createHeaderString(
    HTTPMethod method,
    const URLParseResult& parsedURL,
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

    ss << " ";
    if (parsedURL.path.empty()) {
        ss << "/";
    }
    else {
        ss << parsedURL.path;
    }

    ss << " HTTP/1.1"
       << "\r\n";

    const auto explicitPortNumber = [&]() -> bool {
        // NOTE: The Host header leaves out the default port of the scheme, 80
        // for http and 443 for https. See RFC 9110, section 7.2.
        if (parsedURL.port.empty()) {
            return false;
        }
        if ((parsedURL.protocolScheme == "https://") && (parsedURL.port == "443")) {
            return false;
        }
        if ((parsedURL.protocolScheme == "http://") && (parsedURL.port == "80")) {
            return false;
        }
        return true;
    }();

    ss << "Host: " << parsedURL.hostName;
    if (explicitPortNumber) {
        ss << ":" << parsedURL.port << "\r\n";
    }
    else {
        ss << "\r\n";
    }

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

    for (const auto& field : fields) {
        ss << field.first << ": " << field.second << "\r\n";
    }

    ss << "\r\n";
    return ss.str();
}

/// Returns the address to connect to for parsedURL, adding the default port
/// of the scheme, such as "example.com:443" for "https://example.com".
[[nodiscard]] std::string
makeSocketAddress(const URLParseResult& parsedURL)
{
    std::ostringstream ss;
    if (parsedURL.port.empty()) {
        ss << parsedURL.hostName << ':';
        if (parsedURL.protocolScheme == "https://") {
            ss << 443;
        }
        else {
            POMDOG_ASSERT(parsedURL.protocolScheme == "http://");
            ss << 80;
        }
    }
    else {
        ss << parsedURL.hostName << ':' << parsedURL.port;
    }
    return ss.str();
}

} // namespace

struct HTTPClientNative::Session final {
    u64 requestID = 0;

    /// The key of the connection, which an IdleConnection keeps.
    std::string connectionKey;

    std::shared_ptr<Stream> stream;

    /// The request header and body, written once the stream connects.
    std::vector<u8> requestBytes;

    HTTPParser parser;

    /// Why the request failed before it started, reported by the next update().
    std::unique_ptr<Error> failure;

    bool persistentConnection = true;
    bool isRequestSent = false;

    /// True if `cancel()` stopped the request before it was sent. The session
    /// then reports no event, and keeps the stream until it connects.
    bool isCanceled = false;

    bool isCompleted = false;
};

HTTPClientNative::HTTPClientNative(const HTTPClientOptions& options) noexcept
    : streamOptions_{.connectTimeout = options.connectTimeout, .idleTimeout = options.idleTimeout}
    , trustStore_(options.trustStore)
{
}

HTTPClientNative::~HTTPClientNative() = default;

u64 HTTPClientNative::send(HTTPRequest&& request)
{
    const auto requestID = nextRequestID_;
    ++nextRequestID_;

    auto session = std::make_unique<Session>();
    session->requestID = requestID;
    session->persistentConnection = request.persistentConnection;

    if (auto result = startSession(*session, request); !result) {
        session->failure = std::move(result.error());
    }
    sessions_.push_back(std::move(session));

    return requestID;
}

std::expected<void, std::unique_ptr<Error>>
HTTPClientNative::startSession(Session& session, const HTTPRequest& request)
{
    auto parsedURL = parseURL(request.url);
    if (!parsedURL) {
        return std::unexpected(errors::wrap(std::move(parsedURL.error()), "invalid url " + request.url));
    }

    const bool useTLS = (parsedURL->protocolScheme == "https://");
    if (!useTLS && (parsedURL->protocolScheme != "http://")) {
        return std::unexpected(errors::make("unsupported protocol scheme"));
    }
    if (useTLS && (trustStore_ == nullptr)) {
        return std::unexpected(errors::make("https requests need HTTPClientOptions::trustStore"));
    }

    const auto socketAddress = makeSocketAddress(*parsedURL);
    session.connectionKey = std::string{parsedURL->protocolScheme} + socketAddress;

    std::optional<size_t> contentLength;
    if (!request.body.empty()) {
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
        idleConnections_.erase(idle);
        return {};
    }

    auto stream = useTLS
                      ? connectTLSStream(socketAddress, trustStore_, streamOptions_)
                      : connectTCPStream(socketAddress, streamOptions_);
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
        if (((*session)->stream != nullptr) && !(*session)->isRequestSent) {
            // NOTE: Destroying a stream that is still connecting waits for the
            // connection. The session keeps the stream until it connects, and
            // leaves it to a later request, since nothing was sent.
            (*session)->isCanceled = true;
        }
        else {
            // NOTE: The response may be half read, so the connection is not reused.
            if ((*session)->stream != nullptr) {
                (*session)->stream->close();
            }
            sessions_.erase(session);
        }
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

    if (session.isCanceled) {
        while (auto event = stream.pollEvent()) {
            if (event->kind == StreamEventKind::Disconnected) {
                session.stream = nullptr;
                session.isCompleted = true;
                return;
            }
        }
        if (stream.isConnected()) {
            idleConnections_.push_back(IdleConnection{
                .key = std::move(session.connectionKey),
                .stream = std::move(session.stream),
            });
            session.isCompleted = true;
        }
        return;
    }

    while (auto event = stream.pollEvent()) {
        switch (event->kind) {
        case StreamEventKind::Connected:
            break;
        case StreamEventKind::Received: {
            auto result = session.parser.parse(event->data);
            if (!result) {
                stream.close();
                complete(session, std::unexpected(std::move(result.error())));
                return;
            }
            if (*result == HTTPParseResult::EndOfFile) {
                if (session.persistentConnection && stream.isConnected()) {
                    idleConnections_.push_back(IdleConnection{
                        .key = std::move(session.connectionKey),
                        .stream = std::move(session.stream),
                    });
                }
                else {
                    stream.close();
                }
                complete(session, session.parser.takeResponse());
                return;
            }
            break;
        }
        case StreamEventKind::Disconnected: {
            auto err = (event->error != nullptr)
                           ? errors::wrap(std::move(event->error), "HTTP request error")
                           : errors::make("HTTP request disconnect");
            complete(session, std::unexpected(std::move(err)));
            return;
        }
        }
    }

    if (!session.isRequestSent && stream.isConnected()) {
        session.isRequestSent = true;
        if (auto result = stream.write(session.requestBytes); !result) {
            stream.close();
            complete(session, std::unexpected(std::move(result.error())));
            return;
        }
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
