// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/posix/tcp_stream_posix.h"
#include "pomdog/network/array_view.h"
#include "pomdog/network/io_service.h"
#include "pomdog/network/posix/socket_helper_posix.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <array>
#include <cstring>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
namespace {

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

TCPStreamPOSIX::TCPStreamPOSIX(IOService* serviceIn)
    : service_(serviceIn)
{
}

TCPStreamPOSIX::~TCPStreamPOSIX()
{
    if (blockingThread_.joinable()) {
        blockingThread_.join();
    }

    if (isSocketValid(descriptor_)) {
        ::close(descriptor_);
        descriptor_ = InvalidSocket;
    }
}

void TCPStreamPOSIX::close()
{
    isConnected_ = false;
    eventLoopConn_.disconnect();
    errorConn_.disconnect();

    if (isSocketValid(descriptor_)) {
        ::close(descriptor_);
        descriptor_ = InvalidSocket;
    }
}

std::unique_ptr<Error>
TCPStreamPOSIX::connect(std::string_view host, std::string_view port, const Duration& connectTimeout)
{
    POMDOG_ASSERT(service_ != nullptr);

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    auto hostBuf = std::string{host};
    auto portBuf = std::string{port};

    std::thread connectThread([this, hostBuf = std::move(hostBuf), portBuf = std::move(portBuf), connectTimeout = connectTimeout] {
        auto [fd, err] = detail::connectSocketPOSIX(hostBuf, portBuf, SocketProtocol::TCP, connectTimeout);

        if (err != nullptr) {
            auto wrapped = errors::wrap(std::move(err), "couldn't connect to TCP socket on " + hostBuf + ":" + portBuf);
            std::shared_ptr<Error> shared = std::move(wrapped);
            errorConn_ = service_->scheduleTask([this, sharedErr = std::move(shared)] {
                onConnected(sharedErr->clone());
                errorConn_.disconnect();
            });
            return;
        }

        descriptor_ = fd;
        isConnected_ = true;

        eventLoopConn_ = service_->scheduleTask([this] {
            // NOTE: Update timestamp of last connection
            lastActiveTime_ = service_->getNowTime();

            onConnected(nullptr);
            eventLoopConn_.disconnect();
            eventLoopConn_ = service_->scheduleTask([this] { readEventLoop(); });
        });
    });

    blockingThread_ = std::move(connectThread);

    return nullptr;
}

std::unique_ptr<Error>
TCPStreamPOSIX::write(const ArrayView<std::uint8_t const>& data)
{
    POMDOG_ASSERT(isSocketValid(descriptor_));
    POMDOG_ASSERT(data.data() != nullptr);
    POMDOG_ASSERT(!data.empty());

    auto result = ::send(descriptor_, data.data(), data.size(), flags);

    if (result == -1) {
        const auto errorCode = detail::toErrc(errno);
        return errors::makeIOError(errorCode, "write failed with error");
    }

    // NOTE: Update timestamp of last read/write
    lastActiveTime_ = service_->getNowTime();

    return nullptr;
}

bool TCPStreamPOSIX::isConnected() const noexcept
{
    return isConnected_;
}

void TCPStreamPOSIX::setTimeout(const Duration& timeoutIn)
{
    POMDOG_ASSERT(timeoutIn >= std::remove_reference_t<decltype(timeoutIn)>::zero());
    timeoutInterval_ = timeoutIn;
}

int TCPStreamPOSIX::getHandle() const noexcept
{
    return descriptor_;
}

void TCPStreamPOSIX::readEventLoop()
{
    POMDOG_ASSERT(isSocketValid(descriptor_));

    if (timeoutInterval_.has_value()) {
        if ((service_->getNowTime() - lastActiveTime_) > *timeoutInterval_) {
            onRead({}, errors::make("timeout socket connection"));
            close();
            onDisconnect();
            return;
        }
    }

    // NOTE: Read per 1 frame (= 1/60 seconds) for a packet up to 2048 bytes.
    std::array<std::uint8_t, 2048> buffer;

    ssize_t readSize = ::recv(descriptor_, buffer.data(), buffer.size(), flags);
    if (readSize < 0) {
        const auto errorCode = detail::toErrc(errno);
        if (errorCode == std::errc::resource_unavailable_try_again || errorCode == std::errc::operation_would_block) {
            // NOTE: There is no data to be read yet
            return;
        }

        onRead({}, errors::makeIOError(errorCode, "read failed with error"));
        return;
    }

    // NOTE: Update timestamp of last read/write
    lastActiveTime_ = service_->getNowTime();

    POMDOG_ASSERT(readSize >= 0);
    auto view = ArrayView<std::uint8_t>{buffer.data(), static_cast<std::size_t>(readSize)};
    onRead(std::move(view), nullptr);

    if (readSize == 0) {
        // NOTE: When the peer socket has performed orderly shutdown,
        // the read size will be 0 (meaning the "end-of-file").
        close();
        onDisconnect();
    }
}

} // namespace pomdog::detail
