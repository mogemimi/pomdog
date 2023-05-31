// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/http_client.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/network/array_view.h"
#include "pomdog/network/http_method.h"
#include "pomdog/network/http_parser.h"
#include "pomdog/network/http_request.h"
#include "pomdog/network/http_response.h"
#include "pomdog/network/io_service.h"
#include "pomdog/network/tcp_stream.h"
#include "pomdog/network/tls_stream.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <sstream>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

struct URLParseResult final {
    std::string_view protocolScheme;
    std::string_view hostName;
    std::string_view port;
    std::string_view path;
};

[[nodiscard]] std::tuple<URLParseResult, std::unique_ptr<Error>>
parseURL(std::string_view url)
{
    std::string_view source = url;
    URLParseResult result;

    if (auto iter = source.find("://"); iter == std::string_view::npos) {
        return std::make_tuple(std::move(result), errors::make("no protocol scheme (i.e. 'https://' or 'http://') in request URL"));
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
        return std::make_tuple(std::move(result), errors::make("no Host in request URL"));
    }

    source = result.hostName;

    if (auto iter = source.find(":"); iter != std::string_view::npos) {
        result.hostName = source.substr(0, iter);
        result.port = source.substr(iter);

        if (result.port.size() <= 1) {
            return std::make_tuple(std::move(result), errors::make("no Port in request URL"));
        }
        result.port.remove_prefix(1);
    }

    return std::make_tuple(std::move(result), nullptr);
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
        // NOTE: This implement strip the port number from the Host header when
        // the port is the standard (http port 80, https port 443). Please see
        // https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html#sec14.23
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

[[nodiscard]] std::string
makeSocketAddress(const URLParseResult& parsedURL)
{
    // NOTE: e.g. "https://example.com" to "example.com:443"
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

class SessionKeeper final {
public:
    std::function<void()> updateSessions;
    std::function<void(std::string&& address, TLSStream&& stream)> keepAliveTLS;
    std::function<void(std::string&& address, TCPStream&& stream)> keepAliveTCP;
};

void keepAlive(const std::shared_ptr<SessionKeeper>& keeper, std::string&& address, TLSStream&& stream)
{
    POMDOG_ASSERT(keeper != nullptr);
    keeper->keepAliveTLS(std::move(address), std::move(stream));
}

void keepAlive(const std::shared_ptr<SessionKeeper>& keeper, std::string&& address, TCPStream&& stream)
{
    POMDOG_ASSERT(keeper != nullptr);
    keeper->keepAliveTCP(std::move(address), std::move(stream));
}

class Session {
public:
    virtual ~Session() = default;

    [[nodiscard]] virtual bool
    isCompleted() const noexcept = 0;

    [[nodiscard]] virtual std::unique_ptr<Error>
    abort() = 0;

    [[nodiscard]] virtual const std::shared_ptr<HTTPRequest>&
    getRequest() const = 0;
};

template <class SocketStream>
class HTTPSession final : public Session {
private:
    SocketStream stream_;
    detail::HTTPParser parser_;
    std::shared_ptr<HTTPRequest> request_;
    std::shared_ptr<SessionKeeper> keeper_;
    std::string socketAddress_;
    ScopedConnection connectedConn_;
    ScopedConnection disconnectConn_;
    ScopedConnection readConn_;
    bool isCompleted_ = false;

public:
    void createSession(
        SocketStream&& streamIn,
        const std::shared_ptr<HTTPRequest>& requestIn,
        const std::shared_ptr<SessionKeeper>& keeperIn,
        std::string&& socketAddressIn)
    {
        stream_ = std::move(streamIn);
        request_ = requestIn;
        keeper_ = keeperIn;
        socketAddress_ = std::move(socketAddressIn);

        POMDOG_ASSERT(request_ != nullptr);

        stream_.setTimeout(std::chrono::seconds{5});

        auto sendRequest = [this](const std::unique_ptr<Error>& connErr) {
            if (connErr != nullptr) {
                POMDOG_ASSERT(request_ != nullptr);
                complete(nullptr, errors::wrap(connErr->clone(), "HTTP request error"));
                return;
            }

            POMDOG_ASSERT(request_ != nullptr);

            auto [parsedURL, parseErr] = parseURL(request_->URL);
            if (parseErr != nullptr) {
                auto err = errors::wrap(std::move(parseErr), "invalid url " + request_->URL);
                keepAlive(keeper_, std::move(socketAddress_), std::move(stream_));
                complete(nullptr, std::move(err));
                return;
            }

            std::optional<size_t> contentLength;
            if (!request_->Body.empty()) {
                contentLength = request_->Body.size();
            }

            std::string header = createHeaderString(
                request_->Method,
                parsedURL,
                request_->PersistentConnection,
                request_->Headers,
                contentLength);

            // NOTE: Write the HTTP request
            auto headerView = ArrayView<char const>{header.data(), header.size()};
            if (auto err = stream_.write(headerView.viewAs<std::uint8_t const>()); err != nullptr) {
                complete(nullptr, std::move(err));
                stream_.disconnect();
                return;
            }

            auto& body = request_->Body;
            if (!body.empty()) {
                auto bodyView = ArrayView<char const>{body.data(), body.size()};
                if (auto err = stream_.write(bodyView.viewAs<std::uint8_t const>()); err != nullptr) {
                    complete(nullptr, std::move(err));
                    stream_.disconnect();
                    return;
                }
            }
        };

        if (stream_.isConnected()) {
            // NOTE: Already connected
            sendRequest(nullptr);
            POMDOG_ASSERT(!connectedConn_.IsConnected());
        }
        else {
            connectedConn_ = stream_.onConnected(std::move(sendRequest));
        }

        readConn_ = stream_.onRead([this](const ArrayView<std::uint8_t>& view, const std::unique_ptr<Error>& readErr) {
            if (readErr != nullptr) {
                POMDOG_ASSERT(request_ != nullptr);
                complete(nullptr, errors::wrap(readErr->clone(), "HTTP request error"));
                stream_.disconnect();
                return;
            }

            auto [result, err] = parser_.Parse(view);
            if (err != nullptr) {
                complete(nullptr, std::move(err));
                stream_.disconnect();
                return;
            }

            POMDOG_ASSERT(result != detail::HTTPParseResult::Error);

            if (result == detail::HTTPParseResult::WouldBlock) {
                return;
            }

            POMDOG_ASSERT(result == detail::HTTPParseResult::EndOfFile);
            POMDOG_ASSERT(request_ != nullptr);

            auto response = parser_.GetResponse();
            response->Request = std::move(request_);
            request_ = nullptr;

            keepAlive(keeper_, std::move(socketAddress_), std::move(stream_));
            complete(std::move(response), nullptr);
        });

        disconnectConn_ = stream_.onDisconnect([this] {
            POMDOG_ASSERT(request_ != nullptr);
            complete(nullptr, errors::make("HTTP request disconnect"));
        });
    }

    bool isCompleted() const noexcept override
    {
        return isCompleted_;
    }

    std::unique_ptr<Error> abort() override
    {
        if (isCompleted_) {
            return nullptr;
        }

        if (stream_.isConnected()) {
            connectedConn_.Disconnect();
            disconnectConn_.Disconnect();
            readConn_.Disconnect();
            stream_.disconnect();
            complete(nullptr, errors::make("HTTP request abort"));
            return nullptr;
        }

        connectedConn_ = stream_.onConnected([this](const std::unique_ptr<Error>& err) {
            if (err != nullptr) {
                POMDOG_ASSERT(request_ != nullptr);
                complete(nullptr, errors::wrap(err->clone(), "HTTP request error"));
                return;
            }

            keepAlive(keeper_, std::move(socketAddress_), std::move(stream_));
            complete(nullptr, errors::make("HTTP request abort"));
        });

        readConn_.Disconnect();

        disconnectConn_ = stream_.onDisconnect([this] {
            POMDOG_ASSERT(request_ != nullptr);
            complete(nullptr, errors::make("HTTP request disconnect"));
        });

        return nullptr;
    }

    [[nodiscard]] const std::shared_ptr<HTTPRequest>&
    getRequest() const override
    {
        return request_;
    }

    void complete(std::unique_ptr<HTTPResponse>&& response, std::unique_ptr<Error>&& err)
    {
        connectedConn_.Disconnect();
        disconnectConn_.Disconnect();
        readConn_.Disconnect();

        if (response != nullptr) {
            POMDOG_ASSERT(request_ == nullptr);
            POMDOG_ASSERT(response->Request != nullptr);
            auto req = response->Request;
            req->OnCompleted(std::move(response), std::move(err));
            req->OnCompleted.Disconnect();
        }
        else {
            POMDOG_ASSERT(request_ != nullptr);
            request_->OnCompleted(nullptr, std::move(err));
            request_->OnCompleted.Disconnect();
        }

        isCompleted_ = true;
        keeper_->updateSessions();
    }
};

} // namespace

struct HTTPClient::Impl final {
    unsafe_ptr<IOService> service_ = nullptr;
    std::vector<std::unique_ptr<Session>> sessions_;
    std::shared_ptr<SessionKeeper> sessionKeeper_;
    ScopedConnection updateConn_;

    std::unordered_map<std::string, TLSStream> tlsStreams_;
    std::unordered_map<std::string, TCPStream> tcpStreams_;

    explicit Impl(IOService* service);

    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;

    ~Impl();

    std::unique_ptr<Error>
    Do(const std::shared_ptr<HTTPRequest>& req);

    std::tuple<Connection, std::unique_ptr<Error>>
    Get(const std::string& url,
        std::function<void(std::unique_ptr<HTTPResponse>&&, const std::unique_ptr<Error>&)>&& callback);

    std::tuple<Connection, std::unique_ptr<Error>>
    Post(const std::string& url,
        const std::string& contentType,
        std::vector<char>&& body,
        std::function<void(std::unique_ptr<HTTPResponse>&&, const std::unique_ptr<Error>&)>&& callback);

    std::unique_ptr<Error>
    CancelRequest(const std::shared_ptr<HTTPRequest>& req);
};

HTTPClient::Impl::Impl(IOService* serviceIn)
    : service_(serviceIn)
{
    sessionKeeper_ = std::make_shared<SessionKeeper>();
    sessionKeeper_->updateSessions = [this]() {
        POMDOG_ASSERT(service_ != nullptr);

        if (updateConn_.IsConnected()) {
            return;
        }

        updateConn_ = service_->scheduleTask([this] {
            updateConn_.Disconnect();

            sessions_.erase(
                std::remove_if(std::begin(sessions_), std::end(sessions_), [&](const auto& p) -> bool { return p->isCompleted(); }),
                std::end(sessions_));
        });
    };
    sessionKeeper_->keepAliveTLS = [this](std::string&& address, TLSStream&& stream) {
        tlsStreams_.emplace(std::move(address), std::move(stream));
    };
    sessionKeeper_->keepAliveTCP = [this](std::string&& address, TCPStream&& stream) {
        tcpStreams_.emplace(std::move(address), std::move(stream));
    };
}

HTTPClient::Impl::~Impl()
{
    sessionKeeper_->updateSessions = []() {};
    sessionKeeper_->keepAliveTLS = [](std::string&&, TLSStream&&) {};
    sessionKeeper_->keepAliveTCP = [](std::string&&, TCPStream&&) {};
}

std::unique_ptr<Error>
HTTPClient::Impl::Do(const std::shared_ptr<HTTPRequest>& req)
{
    POMDOG_ASSERT(service_ != nullptr);
    POMDOG_ASSERT(req != nullptr);

    auto [parsedURL, parseErr] = parseURL(req->URL);
    if (parseErr != nullptr) {
        return errors::wrap(std::move(parseErr), "invalid url " + req->URL);
    }

    auto socketAddress = makeSocketAddress(parsedURL);

    if (parsedURL.protocolScheme == "https://") {
        auto session = std::make_unique<HTTPSession<TLSStream>>();
        TLSStream sessionStream;

        if (auto kv = tlsStreams_.find(socketAddress); kv != std::end(tlsStreams_)) {
            sessionStream = std::move(kv->second);
            tlsStreams_.erase(kv);
        }

        if (!sessionStream.isConnected()) {
            auto [stream, err] = TLSStream::connect(service_, socketAddress);
            if (err != nullptr) {
                return errors::wrap(std::move(err), "failed to connect to server");
            }
            sessionStream = std::move(stream);
        }

        session->createSession(std::move(sessionStream), req, sessionKeeper_, std::move(socketAddress));

        sessions_.emplace_back(std::move(session));
    }
    else if (parsedURL.protocolScheme == "http://") {
        auto session = std::make_unique<HTTPSession<TCPStream>>();
        TCPStream sessionStream;

        if (auto kv = tcpStreams_.find(socketAddress); kv != std::end(tcpStreams_)) {
            sessionStream = std::move(kv->second);
            tcpStreams_.erase(kv);
        }

        if (!sessionStream.isConnected()) {
            auto [stream, err] = TCPStream::connect(service_, socketAddress);
            if (err != nullptr) {
                return errors::wrap(std::move(err), "failed to connect to server");
            }
            sessionStream = std::move(stream);
        }

        session->createSession(std::move(sessionStream), req, sessionKeeper_, std::move(socketAddress));

        sessions_.emplace_back(std::move(session));
    }
    else {
        // error
        return errors::make("unsupported protocol scheme");
    }

    return nullptr;
}

std::tuple<Connection, std::unique_ptr<Error>>
HTTPClient::Impl::Get(
    const std::string& url,
    std::function<void(std::unique_ptr<HTTPResponse>&&, const std::unique_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(service_ != nullptr);

    auto request = HTTPRequest::Create(HTTPMethod::Get, url);
    auto conn = request->OnCompleted.Connect(std::move(callback));

    return std::make_tuple(std::move(conn), this->Do(request));
}

std::tuple<Connection, std::unique_ptr<Error>>
HTTPClient::Impl::Post(
    const std::string& url,
    const std::string& contentType,
    std::vector<char>&& body,
    std::function<void(std::unique_ptr<HTTPResponse>&&, const std::unique_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(service_ != nullptr);

    auto request = HTTPRequest::Create(HTTPMethod::Post, url);
    request->AddHeader("Content-Type", contentType);
    request->Body = std::move(body);
    auto conn = request->OnCompleted.Connect(std::move(callback));
    return std::make_tuple(std::move(conn), this->Do(request));
}

std::unique_ptr<Error>
HTTPClient::Impl::CancelRequest(const std::shared_ptr<HTTPRequest>& req)
{
    POMDOG_ASSERT(service_ != nullptr);
    POMDOG_ASSERT(req != nullptr);

    std::unique_ptr<Error> abortErr;

    for (auto& session : sessions_) {
        if (session->isCompleted()) {
            continue;
        }
        if (session->getRequest() == req) {
            if (auto err = session->abort(); err != nullptr) {
                abortErr = std::move(err);
            }
        }
    }

    return abortErr;
}

HTTPClient::HTTPClient(IOService* serviceIn)
    : impl(std::make_unique<Impl>(serviceIn))
{
}

HTTPClient::~HTTPClient()
{
}

std::unique_ptr<Error>
HTTPClient::Do(const std::shared_ptr<HTTPRequest>& req)
{
    POMDOG_ASSERT(impl != nullptr);
    return impl->Do(req);
}

std::tuple<Connection, std::unique_ptr<Error>>
HTTPClient::Get(
    const std::string& url,
    std::function<void(std::unique_ptr<HTTPResponse>&&, const std::unique_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(impl != nullptr);
    return impl->Get(url, std::move(callback));
}

std::tuple<Connection, std::unique_ptr<Error>>
HTTPClient::Post(
    const std::string& url,
    const std::string& contentType,
    std::vector<char>&& body,
    std::function<void(std::unique_ptr<HTTPResponse>&&, const std::unique_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(impl != nullptr);
    return impl->Post(url, contentType, std::move(body), std::move(callback));
}

std::unique_ptr<Error>
HTTPClient::CancelRequest(const std::shared_ptr<HTTPRequest>& req)
{
    POMDOG_ASSERT(impl != nullptr);
    return impl->CancelRequest(req);
}

} // namespace pomdog
