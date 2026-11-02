// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/win32/tcp_stream_win32.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/network/socket_protocol.h"
#include "pomdog/network/win32/socket_helper_win32.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
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
    // NOTE: Wait for a connection in progress, and close its socket.
    if (connectResult_.valid()) {
        if (auto result = connectResult_.get(); result) {
            ::closesocket(*result);
        }
    }
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
    connectResult_ = std::async(
        std::launch::async,
        [host = std::string{host}, port = std::string{port}, connectTimeout = options.connectTimeout]() -> std::expected<::SOCKET, std::unique_ptr<Error>> {
            auto result = connectSocketWin32(host, port, SocketProtocol::TCP, connectTimeout);
            if (!result) {
                return std::unexpected(errors::wrap(std::move(result.error()), "couldn't connect to TCP socket on " + host + ":" + port));
            }
            return result;
        });

    return {};
}

void TCPStreamWin32::update()
{
    if (connectResult_.valid()) {
        if (connectResult_.wait_for(std::chrono::seconds{0}) != std::future_status::ready) {
            return;
        }

        auto result = connectResult_.get();
        if (isClosed_) {
            // NOTE: The stream was closed while connecting.
            if (result) {
                ::closesocket(*result);
            }
            return;
        }
        if (!result) {
            disconnect(std::move(result.error()));
            return;
        }

        descriptor_ = *result;
        lastActiveTime_ = Clock::now();
        events_.push_back(StreamEvent{.kind = StreamEventKind::Connected, .data = {}, .error = nullptr});
    }

    if (isClosed_ || !isSocketValid(descriptor_)) {
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
    if (data.empty()) {
        return {};
    }

    auto result = ::send(
        descriptor_,
        reinterpret_cast<const char*>(data.data()),
        static_cast<int>(data.size()),
        0);

    if (result == SOCKET_ERROR) {
        return std::unexpected(errors::make("send failed with error: " + std::to_string(::WSAGetLastError())));
    }

    // NOTE: Update timestamp of last read/write
    lastActiveTime_ = Clock::now();

    return {};
}

void TCPStreamWin32::close()
{
    isClosed_ = true;
    closeSocket();
    events_.clear();
}

bool TCPStreamWin32::isConnected() const noexcept
{
    return !isClosed_ && isSocketValid(descriptor_);
}

void TCPStreamWin32::disconnect(std::unique_ptr<Error>&& err)
{
    isClosed_ = true;
    closeSocket();
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

    // NOTE: Read per 1 frame (= 1/60 seconds) for a packet up to 2048 bytes.
    std::array<u8, 2048> buffer;

    const auto readSize = ::recv(descriptor_, reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), 0);
    if (readSize == SOCKET_ERROR) {
        const auto errorCode = ::WSAGetLastError();
        if (errorCode == WSAEWOULDBLOCK) {
            // NOTE: There is no data to be read yet
            return;
        }

        disconnect(errors::make("read failed with error: " + std::to_string(errorCode)));
        return;
    }

    if (readSize == 0) {
        // NOTE: When the peer socket has performed orderly shutdown,
        // the read size will be 0 (meaning the "end-of-file").
        disconnect(nullptr);
        return;
    }

    // NOTE: Update timestamp of last read/write
    lastActiveTime_ = Clock::now();

    POMDOG_ASSERT(readSize > 0);
    const auto view = std::span{buffer}.first(static_cast<std::size_t>(readSize));
    events_.push_back(StreamEvent{.kind = StreamEventKind::Received, .data = std::vector<u8>(view.begin(), view.end()), .error = nullptr});
}

} // namespace pomdog::network::detail
