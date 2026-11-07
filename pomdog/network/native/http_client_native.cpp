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

    // NOTE: The client decodes no content coding. Without the header, the
    // server could choose any (RFC 9110, section 12.5.3).
    ss << "Accept-Encoding: identity\r\n";

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
            const auto comma = codings.find(',');
            auto coding = codings.substr(0, comma);
            codings = (comma == std::string_view::npos) ? std::string_view{} : codings.substr(comma + 1);
            while (!coding.empty() && ((coding.front() == ' ') || (coding.front() == '\t'))) {
                coding.remove_prefix(1);
            }
            while (!coding.empty() && ((coding.back() == ' ') || (coding.back() == '\t'))) {
                coding.remove_suffix(1);
            }
            if (!coding.empty() && !equalsIgnoringCase(coding, "identity")) {
                return std::unexpected(errors::make("the response has the content coding " + std::string{coding} + ", which the client does not decode"));
            }
        }
    }
    return {};
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

    if (request.credentialsMode == HTTPCredentialsMode::SameOrigin) {
        return std::unexpected(errors::make("HTTPCredentialsMode::SameOrigin needs the origin of a web page, which native platforms do not have"));
    }

    // NOTE: A fragment names a part of the resource for the client, and is
    // not sent (RFC 9110, section 4.2.4).
    auto parsedURL = parseURL(removeURLFragment(request.url));
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
    if (auto result = checkRequestMethod(request.method); !result) {
        return std::unexpected(std::move(result.error()));
    }
    if (auto result = checkRequestHeaders(request.headers); !result) {
        return std::unexpected(std::move(result.error()));
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
            auto result = parser.parse(event->data);
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
                finishResponse(session, parser.takeResponse());
                return;
            }
            break;
        }
        case StreamEventKind::Disconnected: {
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
