// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/posix/tcp_stream_posix.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/network/posix/socket_helper_posix.h"
#include "pomdog/network/socket_protocol.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <array>
#include <cerrno>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

using Clock = std::chrono::steady_clock;

constexpr int InvalidSocket = -1;

bool isSocketValid(int descriptor) noexcept
{
    return descriptor >= 0;
}

#if defined(POMDOG_PLATFORM_LINUX)
constexpr int flags = MSG_NOSIGNAL;
#else
constexpr int flags = 0;
#endif

} // namespace

TCPStreamPOSIX::TCPStreamPOSIX() noexcept = default;

TCPStreamPOSIX::~TCPStreamPOSIX()
{
    // NOTE: Wait for a connection in progress, and close its socket.
    if (connectResult_.valid()) {
        if (auto result = connectResult_.get(); result) {
            ::close(*result);
        }
    }
    closeSocket();
}

std::expected<void, std::unique_ptr<Error>>
TCPStreamPOSIX::connect(std::string_view host, std::string_view port, const StreamOptions& options)
{
    idleTimeout_ = options.idleTimeout;

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    connectResult_ = std::async(
        std::launch::async,
        [host = std::string{host}, port = std::string{port}, connectTimeout = options.connectTimeout]() -> std::expected<int, std::unique_ptr<Error>> {
            auto result = connectSocketPOSIX(host, port, SocketProtocol::TCP, connectTimeout);
            if (!result) {
                return std::unexpected(errors::wrap(std::move(result.error()), "couldn't connect to TCP socket on " + host + ":" + port));
            }
            return result;
        });

    return {};
}

void TCPStreamPOSIX::update()
{
    if (connectResult_.valid()) {
        if (connectResult_.wait_for(std::chrono::seconds{0}) != std::future_status::ready) {
            return;
        }

        auto result = connectResult_.get();
        if (isClosed_) {
            // NOTE: The stream was closed while connecting.
            if (result) {
                ::close(*result);
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
TCPStreamPOSIX::pollEvent()
{
    if (events_.empty()) {
        return std::nullopt;
    }
    auto event = std::move(events_.front());
    events_.pop_front();
    return event;
}

std::expected<void, std::unique_ptr<Error>>
TCPStreamPOSIX::write(std::span<const u8> data)
{
    if (!isConnected()) {
        return std::unexpected(errors::make("the TCP stream is not connected"));
    }

    sendBuffer_.insert(std::end(sendBuffer_), std::begin(data), std::end(data));
    flushSendBuffer();
    return {};
}

void TCPStreamPOSIX::flushSendBuffer()
{
    POMDOG_ASSERT(isSocketValid(descriptor_));

    std::size_t sentSize = 0;
    while (sentSize < sendBuffer_.size()) {
        const auto rest = std::span{sendBuffer_}.subspan(sentSize);

        const auto result = ::send(descriptor_, rest.data(), rest.size(), flags);
        if (result < 0) {
            const auto errorCode = errors::toErrc(errno);
            if ((errorCode == std::errc::resource_unavailable_try_again) ||
                (errorCode == std::errc::operation_would_block) ||
                (errorCode == std::errc::interrupted)) {
                // NOTE: The socket takes no more bytes now. update() sends the rest.
                break;
            }
            disconnect(errors::fromErrc(errorCode, "write failed with error"));
            return;
        }
        sentSize += static_cast<std::size_t>(result);

        // NOTE: Update timestamp of last read/write
        lastActiveTime_ = Clock::now();
    }
    sendBuffer_.erase(std::begin(sendBuffer_), std::next(std::begin(sendBuffer_), static_cast<std::ptrdiff_t>(sentSize)));
}

void TCPStreamPOSIX::close()
{
    isClosed_ = true;
    closeSocket();
    events_.clear();
    sendBuffer_.clear();
}

bool TCPStreamPOSIX::isConnected() const noexcept
{
    return !isClosed_ && isSocketValid(descriptor_);
}

void TCPStreamPOSIX::disconnect(std::unique_ptr<Error>&& err)
{
    isClosed_ = true;
    closeSocket();
    sendBuffer_.clear();
    events_.push_back(StreamEvent{.kind = StreamEventKind::Disconnected, .data = {}, .error = std::move(err)});
}

void TCPStreamPOSIX::closeSocket() noexcept
{
    if (isSocketValid(descriptor_)) {
        ::close(descriptor_);
        descriptor_ = InvalidSocket;
    }
}

void TCPStreamPOSIX::readSocket()
{
    POMDOG_ASSERT(isSocketValid(descriptor_));

    // NOTE: Read per 1 frame (= 1/60 seconds) for a packet up to 2048 bytes.
    std::array<u8, 2048> buffer;

    const ssize_t readSize = ::recv(descriptor_, buffer.data(), buffer.size(), flags);
    if (readSize < 0) {
        const auto errorCode = errors::toErrc(errno);
        if (errorCode == std::errc::resource_unavailable_try_again || errorCode == std::errc::operation_would_block) {
            // NOTE: There is no data to be read yet
            return;
        }

        disconnect(errors::fromErrc(errorCode, "read failed with error"));
        return;
    }

    if (readSize == 0) {
        // NOTE: When the peer socket has performed orderly shutdown,
        // the read size will be 0 (meaning the "end-of-file").
        disconnect(nullptr);
        return;
    }

    if (static_cast<std::size_t>(readSize) > buffer.size()) {
        disconnect(errors::make("read size is larger than buffer size"));
        return;
    }

    // NOTE: Update timestamp of last read/write
    lastActiveTime_ = Clock::now();

    const auto view = std::span{buffer}.first(static_cast<std::size_t>(readSize));
    events_.push_back(StreamEvent{.kind = StreamEventKind::Received, .data = std::vector<u8>(view.begin(), view.end()), .error = nullptr});
}

} // namespace pomdog::network::detail
