// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/win32/tcp_stream_win32.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/network/receive_limits.h"
#include "pomdog/network/socket_protocol.h"
#include "pomdog/network/win32/socket_helper_win32.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <iterator>
#include <limits>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

using Clock = std::chrono::steady_clock;

[[nodiscard]] bool
isSocketValid(::SOCKET descriptor) noexcept
{
    return descriptor != INVALID_SOCKET;
}

} // namespace

TCPStreamWin32::TCPStreamWin32() noexcept = default;

TCPStreamWin32::~TCPStreamWin32()
{
    // NOTE: A connection in progress does not hold up the destructor; its
    // thread closes the socket that it gets.
    connectWorker_.abandon();
    closeSocket();
}

std::expected<void, std::unique_ptr<Error>>
TCPStreamWin32::connect(std::string_view host, std::string_view port, const StreamOptions& options)
{
    if (auto result = winsock_.startup(); !result) {
        return std::unexpected(std::move(result.error()));
    }

    idleTimeout_ = options.idleTimeout;

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    connectWorker_.start(
        [host = std::string{host}, port = std::string{port}, connectTimeout = options.connectTimeout, winsock = std::make_unique<WinsockLibrary>()]() -> std::expected<::SOCKET, std::unique_ptr<Error>> {
            // NOTE: The thread keeps Winsock started on its own, since it may
            // close the socket after the stream is gone.
            if (auto started = winsock->startup(); !started) {
                return std::unexpected(std::move(started.error()));
            }
            auto result = connectSocketWin32(host, port, SocketProtocol::TCP, connectTimeout);
            if (!result) {
                return std::unexpected(errors::wrap(std::move(result.error()), "couldn't connect to TCP socket on " + host + ":" + port));
            }
            return result;
        },
        [](::SOCKET socket) noexcept { ::closesocket(socket); },
        options.connectTimeout,
        "couldn't connect to TCP socket on " + std::string{host} + ":" + std::string{port});

    return {};
}

void TCPStreamWin32::update()
{
    if (connectWorker_.isRunning()) {
        auto result = connectWorker_.poll();
        if (!result.has_value()) {
            return;
        }
        if (!*result) {
            disconnect(std::move(result->error()));
            return;
        }

        descriptor_ = **result;
        lastActiveTime_ = Clock::now();
        events_.push_back(StreamEvent{.kind = StreamEventKind::Connected, .data = {}, .error = nullptr});
    }

    if (isClosed_ || !isSocketValid(descriptor_)) {
        return;
    }

    flushSendBuffer();
    if (!isConnected()) {
        return;
    }

    if ((idleTimeout_ > Duration::zero()) && ((Clock::now() - lastActiveTime_) > idleTimeout_)) {
        disconnect(errors::make("timeout socket connection"));
        return;
    }

    readSocket();
}

std::optional<StreamEvent>
TCPStreamWin32::pollEvent()
{
    if (events_.empty()) {
        return std::nullopt;
    }
    auto event = std::move(events_.front());
    events_.pop_front();
    return event;
}

std::expected<void, std::unique_ptr<Error>>
TCPStreamWin32::write(std::span<const u8> data)
{
    if (!isConnected()) {
        return std::unexpected(errors::make("the TCP stream is not connected"));
    }

    sendBuffer_.insert(std::end(sendBuffer_), std::begin(data), std::end(data));
    flushSendBuffer();
    return {};
}

void TCPStreamWin32::flushSendBuffer()
{
    POMDOG_ASSERT(isSocketValid(descriptor_));

    std::size_t sentSize = 0;
    while (sentSize < sendBuffer_.size()) {
        const auto rest = std::span{sendBuffer_}.subspan(sentSize);

        // NOTE: send() takes the size as an int, so a larger buffer goes out in parts.
        const auto size = std::min<std::size_t>(rest.size(), static_cast<std::size_t>(std::numeric_limits<int>::max()));

        const auto result = ::send(descriptor_, reinterpret_cast<const char*>(rest.data()), static_cast<int>(size), 0);
        if (result == SOCKET_ERROR) {
            const auto errorCode = ::WSAGetLastError();
            if (errorCode == WSAEWOULDBLOCK) {
                // NOTE: The socket takes no more bytes now. update() sends the rest.
                break;
            }
            disconnect(errors::make("send failed with error: " + std::to_string(errorCode)));
            return;
        }
        sentSize += static_cast<std::size_t>(result);

        // NOTE: Update timestamp of last read/write
        lastActiveTime_ = Clock::now();
    }
    sendBuffer_.erase(std::begin(sendBuffer_), std::next(std::begin(sendBuffer_), static_cast<std::ptrdiff_t>(sentSize)));
}

void TCPStreamWin32::close()
{
    isClosed_ = true;
    connectWorker_.abandon();
    closeSocket();
    events_.clear();
    sendBuffer_.clear();
}

bool TCPStreamWin32::isConnected() const noexcept
{
    return !isClosed_ && isSocketValid(descriptor_);
}

void TCPStreamWin32::disconnect(std::unique_ptr<Error>&& err)
{
    isClosed_ = true;
    closeSocket();
    sendBuffer_.clear();
    events_.push_back(StreamEvent{.kind = StreamEventKind::Disconnected, .data = {}, .error = std::move(err)});
}

void TCPStreamWin32::closeSocket() noexcept
{
    if (isSocketValid(descriptor_)) {
        ::closesocket(descriptor_);
        descriptor_ = INVALID_SOCKET;
    }
}

void TCPStreamWin32::readSocket()
{
    POMDOG_ASSERT(isSocketValid(descriptor_));

    if (receiveBuffer_.empty()) {
        receiveBuffer_.resize(receiveBufferSize);
    }

    // NOTE: Read until the socket has no more bytes, and report them as one
    // event before the stream closes.
    std::vector<u8> data;
    std::unique_ptr<Error> err;
    bool isEndOfStream = false;
    while (data.size() < maxReceiveSizePerUpdate) {
        const auto size = std::min(receiveBuffer_.size(), maxReceiveSizePerUpdate - data.size());
        const auto readSize = ::recv(descriptor_, reinterpret_cast<char*>(receiveBuffer_.data()), static_cast<int>(size), 0);
        if (readSize == SOCKET_ERROR) {
            const auto errorCode = ::WSAGetLastError();
            if (errorCode != WSAEWOULDBLOCK) {
                err = errors::make("read failed with error: " + std::to_string(errorCode));
            }
            break;
        }
        if (readSize == 0) {
            // NOTE: The peer shut the connection down (end of file).
            isEndOfStream = true;
            break;
        }

        POMDOG_ASSERT(readSize > 0);
        const auto view = std::span{receiveBuffer_}.first(static_cast<std::size_t>(readSize));
        data.insert(std::end(data), std::begin(view), std::end(view));
    }

    if (!data.empty()) {
        // NOTE: Update timestamp of last read/write
        lastActiveTime_ = Clock::now();
        events_.push_back(StreamEvent{.kind = StreamEventKind::Received, .data = std::move(data), .error = nullptr});
    }
    if ((err != nullptr) || isEndOfStream) {
        disconnect(std::move(err));
    }
}

} // namespace pomdog::network::detail
