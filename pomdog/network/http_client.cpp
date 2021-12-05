// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Network/HTTPClient.hpp"
#include "HTTPParser.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Network/ArrayView.hpp"
#include "Pomdog/Network/HTTPMethod.hpp"
#include "Pomdog/Network/HTTPRequest.hpp"
#include "Pomdog/Network/HTTPResponse.hpp"
#include "Pomdog/Network/IOService.hpp"
#include "Pomdog/Network/TCPStream.hpp"
#include "Pomdog/Network/TLSStream.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <sstream>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
namespace {

struct URLParseResult final {
    std::string_view protocolScheme;
    std::string_view hostName;
    std::string_view port;
    std::string_view path;
};

std::tuple<URLParseResult, std::unique_ptr<Error>>
ParseURL(std::string_view url)
{
    std::string_view source = url;
    URLParseResult result;

    if (auto iter = source.find("://"); iter == std::string_view::npos) {
        return std::make_tuple(std::move(result), Errors::New("no protocol scheme (i.e. 'https://' or 'http://') in request URL"));
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
        return std::make_tuple(std::move(result), Errors::New("no Host in request URL"));
    }

    source = result.hostName;

    if (auto iter = source.find(":"); iter != std::string_view::npos) {
        result.hostName = source.substr(0, iter);
        result.port = source.substr(iter);

        if (result.port.size() <= 1) {
            return std::make_tuple(std::move(result), Errors::New("no Port in request URL"));
        }
        result.port.remove_prefix(1);
    }

    return std::make_tuple(std::move(result), nullptr);
}

std::string CreateHeaderString(
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

    ss << " HTTP/1.1" << "\r\n";

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
        ss << "Connection: " << "keep-alive" << "\r\n";
    }
    else {
        ss << "Connection: " << "close" << "\r\n";
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

std::string MakeSocketAddress(const URLParseResult& parsedURL)
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
    std::function<void()> UpdateSessions;
    std::function<void(std::string&& address, TLSStream&& stream)> KeepAliveTLS;
    std::function<void(std::string&& address, TCPStream&& stream)> KeepAliveTCP;
};

void KeepAlive(const std::shared_ptr<SessionKeeper>& keeper, std::string&& address, TLSStream&& stream)
{
    POMDOG_ASSERT(keeper != nullptr);
    keeper->KeepAliveTLS(std::move(address), std::move(stream));
}

void KeepAlive(const std::shared_ptr<SessionKeeper>& keeper, std::string&& address, TCPStream&& stream)
{
    POMDOG_ASSERT(keeper != nullptr);
    keeper->KeepAliveTCP(std::move(address), std::move(stream));
}

class Session {
public:
    virtual ~Session() = default;

    virtual bool IsCompleted() const noexcept = 0;

    virtual std::unique_ptr<Error> Abort() = 0;

    virtual const std::shared_ptr<HTTPRequest>& GetRequest() const = 0;
};

template <class SocketStream>
class HTTPSession final : public Session {
private:
    SocketStream stream;
    Detail::HTTPParser parser;
    std::shared_ptr<HTTPRequest> request;
    std::shared_ptr<SessionKeeper> keeper;
    std::string socketAddress;
    ScopedConnection connectedConn;
    ScopedConnection disconnectConn;
    ScopedConnection readConn;
    bool isCompleted = false;

public:
    void CreateSession(
        SocketStream&& stream,
        const std::shared_ptr<HTTPRequest>& request,
        const std::shared_ptr<SessionKeeper>& keeper,
        std::string&& socketAddress);

    bool IsCompleted() const noexcept override
    {
        return isCompleted;
    }

    std::unique_ptr<Error> Abort() override;

    const std::shared_ptr<HTTPRequest>& GetRequest() const override
    {
        return request;
    }

    void Complete(std::unique_ptr<HTTPResponse>&& response, std::unique_ptr<Error>&& err);
};

template <class SocketStream>
void HTTPSession<SocketStream>::CreateSession(
    SocketStream&& streamIn,
    const std::shared_ptr<HTTPRequest>& requestIn,
    const std::shared_ptr<SessionKeeper>& keeperIn,
    std::string&& socketAddressIn)
{
    this->stream = std::move(streamIn);
    this->request = requestIn;
    this->keeper = keeperIn;
    this->socketAddress = std::move(socketAddressIn);

    POMDOG_ASSERT(request != nullptr);

    stream.SetTimeout(std::chrono::seconds{5});

    auto sendRequest = [this](const std::unique_ptr<Error>& connErr) {
        if (connErr != nullptr) {
            POMDOG_ASSERT(request != nullptr);
            this->Complete(nullptr, Errors::Wrap(connErr->Clone(), "HTTP request error"));
            return;
        }

        POMDOG_ASSERT(request != nullptr);

        auto [parsedURL, parseErr] = ParseURL(request->URL);
        if (parseErr != nullptr) {
            auto err = Errors::Wrap(std::move(parseErr), "invalid url " + request->URL);
            KeepAlive(keeper, std::move(socketAddress), std::move(stream));
            this->Complete(nullptr, std::move(err));
            return;
        }

        std::optional<size_t> contentLength;
        if (!request->Body.empty()) {
            contentLength = request->Body.size();
        }

        std::string header = CreateHeaderString(
            request->Method,
            parsedURL,
            request->PersistentConnection,
            request->Headers,
            contentLength);

        // NOTE: Write the HTTP request
        auto headerView = ArrayView<char const>{header.data(), header.size()};
        if (auto err = stream.Write(headerView.ViewAs<std::uint8_t const>()); err != nullptr) {
            this->Complete(nullptr, std::move(err));
            stream.Disconnect();
            return;
        }

        auto& body = request->Body;
        if (!body.empty()) {
            auto bodyView = ArrayView<char const>{body.data(), body.size()};
            if (auto err = stream.Write(bodyView.ViewAs<std::uint8_t const>()); err != nullptr) {
                this->Complete(nullptr, std::move(err));
                stream.Disconnect();
                return;
            }
        }
    };

    if (stream.IsConnected()) {
        // NOTE: Already connected
        sendRequest(nullptr);
        POMDOG_ASSERT(!connectedConn.IsConnected());
    }
    else {
        connectedConn = stream.OnConnected(std::move(sendRequest));
    }

    readConn = stream.OnRead([this](const ArrayView<std::uint8_t>& view, const std::unique_ptr<Error>& readErr) {
        if (readErr != nullptr) {
            POMDOG_ASSERT(request != nullptr);
            this->Complete(nullptr, Errors::Wrap(readErr->Clone(), "HTTP request error"));
            stream.Disconnect();
            return;
        }

        auto [result, err] = parser.Parse(view);
        if (err != nullptr) {
            this->Complete(nullptr, std::move(err));
            stream.Disconnect();
            return;
        }

        POMDOG_ASSERT(result != Detail::HTTPParseResult::Error);

        if (result == Detail::HTTPParseResult::WouldBlock) {
            return;
        }

        POMDOG_ASSERT(result == Detail::HTTPParseResult::EndOfFile);
        POMDOG_ASSERT(request != nullptr);

        auto response = parser.GetResponse();
        response->Request = std::move(request);
        request = nullptr;

        KeepAlive(keeper, std::move(socketAddress), std::move(stream));
        this->Complete(std::move(response), nullptr);
    });

    disconnectConn = stream.OnDisconnect([this] {
        POMDOG_ASSERT(request != nullptr);
        this->Complete(nullptr, Errors::New("HTTP request disconnect"));
    });
}

template <class SocketStream>
void HTTPSession<SocketStream>::Complete(std::unique_ptr<HTTPResponse>&& response, std::unique_ptr<Error>&& err)
{
    connectedConn.Disconnect();
    disconnectConn.Disconnect();
    readConn.Disconnect();

    if (response != nullptr) {
        POMDOG_ASSERT(request == nullptr);
        POMDOG_ASSERT(response->Request != nullptr);
        auto req = response->Request;
        req->OnCompleted(std::move(response), std::move(err));
        req->OnCompleted.Disconnect();
    }
    else {
        POMDOG_ASSERT(request != nullptr);
        request->OnCompleted(nullptr, std::move(err));
        request->OnCompleted.Disconnect();
    }

    isCompleted = true;
    keeper->UpdateSessions();
}

template <class SocketStream>
std::unique_ptr<Error> HTTPSession<SocketStream>::Abort()
{
    if (isCompleted) {
        return nullptr;
    }

    if (stream.IsConnected()) {
        connectedConn.Disconnect();
        disconnectConn.Disconnect();
        readConn.Disconnect();
        stream.Disconnect();
        this->Complete(nullptr, Errors::New("HTTP request abort"));
        return nullptr;
    }

    connectedConn = stream.OnConnected([this](const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            POMDOG_ASSERT(request != nullptr);
            this->Complete(nullptr, Errors::Wrap(err->Clone(), "HTTP request error"));
            return;
        }

        KeepAlive(keeper, std::move(socketAddress), std::move(stream));
        this->Complete(nullptr, Errors::New("HTTP request abort"));
    });

    readConn.Disconnect();

    disconnectConn = stream.OnDisconnect([this] {
        POMDOG_ASSERT(request != nullptr);
        this->Complete(nullptr, Errors::New("HTTP request disconnect"));
    });

    return nullptr;
}

} // namespace

struct HTTPClient::Impl final {
    IOService* service = nullptr;
    std::vector<std::unique_ptr<Session>> sessions;
    std::shared_ptr<SessionKeeper> sessionKeeper;
    ScopedConnection updateConn;

    std::unordered_map<std::string, TLSStream> tlsStreams;
    std::unordered_map<std::string, TCPStream> tcpStreams;

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
    : service(serviceIn)
{
    sessionKeeper = std::make_shared<SessionKeeper>();
    sessionKeeper->UpdateSessions = [this]() {
        POMDOG_ASSERT(service != nullptr);

        if (updateConn.IsConnected()) {
            return;
        }

        updateConn = service->ScheduleTask([this] {
            updateConn.Disconnect();

            sessions.erase(
                std::remove_if(std::begin(sessions), std::end(sessions), [&](const auto& p) -> bool { return p->IsCompleted(); }),
                std::end(sessions));
        });
    };
    sessionKeeper->KeepAliveTLS = [this](std::string&& address, TLSStream&& stream) {
        tlsStreams.emplace(std::move(address), std::move(stream));
    };
    sessionKeeper->KeepAliveTCP = [this](std::string&& address, TCPStream&& stream) {
        tcpStreams.emplace(std::move(address), std::move(stream));
    };
}

HTTPClient::Impl::~Impl()
{
    sessionKeeper->UpdateSessions = []() {};
    sessionKeeper->KeepAliveTLS = [](std::string&&, TLSStream&&) {};
    sessionKeeper->KeepAliveTCP = [](std::string&&, TCPStream&&) {};
}

std::unique_ptr<Error>
HTTPClient::Impl::Do(const std::shared_ptr<HTTPRequest>& req)
{
    POMDOG_ASSERT(service != nullptr);
    POMDOG_ASSERT(req != nullptr);

    auto [parsedURL, parseErr] = ParseURL(req->URL);
    if (parseErr != nullptr) {
        return Errors::Wrap(std::move(parseErr), "invalid url " + req->URL);
    }

    auto socketAddress = MakeSocketAddress(parsedURL);

    if (parsedURL.protocolScheme == "https://") {
        auto session = std::make_unique<HTTPSession<TLSStream>>();
        TLSStream sessionStream;

        if (auto kv = this->tlsStreams.find(socketAddress); kv != std::end(this->tlsStreams)) {
            sessionStream = std::move(kv->second);
            this->tlsStreams.erase(kv);
        }

        if (!sessionStream.IsConnected()) {
            auto [stream, err] = TLSStream::Connect(service, socketAddress);
            if (err != nullptr) {
                return Errors::Wrap(std::move(err), "failed to connect to server");
            }
            sessionStream = std::move(stream);
        }

        session->CreateSession(std::move(sessionStream), req, sessionKeeper, std::move(socketAddress));

        this->sessions.emplace_back(std::move(session));
    }
    else if (parsedURL.protocolScheme == "http://") {
        auto session = std::make_unique<HTTPSession<TCPStream>>();
        TCPStream sessionStream;

        if (auto kv = this->tcpStreams.find(socketAddress); kv != std::end(this->tcpStreams)) {
            sessionStream = std::move(kv->second);
            this->tcpStreams.erase(kv);
        }

        if (!sessionStream.IsConnected()) {
            auto [stream, err] = TCPStream::Connect(service, socketAddress);
            if (err != nullptr) {
                return Errors::Wrap(std::move(err), "failed to connect to server");
            }
            sessionStream = std::move(stream);
        }

        session->CreateSession(std::move(sessionStream), req, sessionKeeper, std::move(socketAddress));

        this->sessions.emplace_back(std::move(session));
    }
    else {
        // error
        return Errors::New("unsupported protocol scheme");
    }

    return nullptr;
}

std::tuple<Connection, std::unique_ptr<Error>>
HTTPClient::Impl::Get(
    const std::string& url,
    std::function<void(std::unique_ptr<HTTPResponse>&&, const std::unique_ptr<Error>&)>&& callback)
{
    POMDOG_ASSERT(service != nullptr);

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
    POMDOG_ASSERT(service != nullptr);

    auto request = HTTPRequest::Create(HTTPMethod::Post, url);
    request->AddHeader("Content-Type", contentType);
    request->Body = std::move(body);
    auto conn = request->OnCompleted.Connect(std::move(callback));
    return std::make_tuple(std::move(conn), this->Do(request));
}

std::unique_ptr<Error>
HTTPClient::Impl::CancelRequest(const std::shared_ptr<HTTPRequest>& req)
{
    POMDOG_ASSERT(service != nullptr);
    POMDOG_ASSERT(req != nullptr);

    for (auto& session : sessions) {
        if (session->IsCompleted()) {
            continue;
        }
        if (session->GetRequest() == req) {
            session->Abort();
        }
    }

    return nullptr;
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

} // namespace Pomdog
