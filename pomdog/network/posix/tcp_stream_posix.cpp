// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/posix/tcp_stream_posix.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/network/posix/socket_helper_posix.h"
#include "pomdog/network/receive_limits.h"
#include "pomdog/network/socket_protocol.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
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
    // NOTE: A connection in progress does not hold up the destructor; its
    // thread closes the socket that it gets.
    connectWorker_.abandon();
    closeSocket();
}

std::expected<void, std::unique_ptr<Error>>
TCPStreamPOSIX::connect(std::string_view host, std::string_view port, const StreamOptions& options)
{
    idleTimeout_ = options.idleTimeout;

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    connectWorker_.start(
        [host = std::string{host}, port = std::string{port}, connectTimeout = options.connectTimeout]() -> std::expected<int, std::unique_ptr<Error>> {
            auto result = connectSocketPOSIX(host, port, SocketProtocol::TCP, connectTimeout);
            if (!result) {
                return std::unexpected(errors::wrap(std::move(result.error()), "couldn't connect to TCP socket on " + host + ":" + port));
            }
            return result;
        },
        [](int socket) noexcept { ::close(socket); },
        options.connectTimeout,
        "couldn't connect to TCP socket on " + std::string{host} + ":" + std::string{port});

    return {};
}

void TCPStreamPOSIX::update()
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

    // NOTE: Read before the idle check, so that bytes that arrived while the
    // game did not call update(), such as during a long load, count.
    readSocket();
    if (!isConnected()) {
        return;
    }

    if ((idleTimeout_ > Duration::zero()) && ((Clock::now() - lastActiveTime_) > idleTimeout_)) {
        disconnect(errors::make("timeout socket connection"));
    }
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
    connectWorker_.abandon();
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
        const ssize_t readSize = ::recv(descriptor_, receiveBuffer_.data(), size, flags);
        if (readSize < 0) {
            const auto errorCode = errors::toErrc(errno);
            if ((errorCode != std::errc::resource_unavailable_try_again) && (errorCode != std::errc::operation_would_block)) {
                err = errors::fromErrc(errorCode, "read failed with error");
            }
            break;
        }
        if (readSize == 0) {
            // NOTE: The peer shut the connection down (end of file).
            isEndOfStream = true;
            break;
        }
        if (static_cast<std::size_t>(readSize) > size) {
            err = errors::make("read size is larger than buffer size");
            break;
        }

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
