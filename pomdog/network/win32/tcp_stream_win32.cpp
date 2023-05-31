// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/win32/tcp_stream_win32.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/network/array_view.h"
#include "pomdog/network/io_service.h"
#include "pomdog/network/win32/socket_helper_win32.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
namespace {

[[nodiscard]] bool
isSocketValid(::SOCKET descriptor) noexcept
{
    return descriptor != INVALID_SOCKET;
}

} // namespace

TCPStreamWin32::TCPStreamWin32(IOService* serviceIn)
    : service_(serviceIn)
{
}

TCPStreamWin32::~TCPStreamWin32()
{
    if (blockingThread_.joinable()) {
        blockingThread_.join();
    }

    if (isSocketValid(descriptor_)) {
        ::closesocket(descriptor_);
        descriptor_ = INVALID_SOCKET;
    }
}

void TCPStreamWin32::close()
{
    isConnected_ = false;
    eventLoopConn_.disconnect();
    errorConn_.disconnect();

    if (isSocketValid(descriptor_)) {
        ::closesocket(descriptor_);
        descriptor_ = INVALID_SOCKET;
    }
}

std::unique_ptr<Error>
TCPStreamWin32::connect(std::string_view host, std::string_view port, const Duration& connectTimeout)
{
    POMDOG_ASSERT(service_ != nullptr);

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    const auto hostBuf = std::string{host};
    const auto portBuf = std::string{port};

    std::thread connectThread([this, hostBuf = std::move(hostBuf), portBuf = std::move(portBuf), connectTimeout = connectTimeout] {
        auto [fd, err] = detail::connectSocketWin32(hostBuf, portBuf, SocketProtocol::TCP, connectTimeout);

        if (err != nullptr) {
            auto wrapped = errors::wrap(std::move(err), "couldn't connect to TCP socket on " + hostBuf + ":" + portBuf);
            std::shared_ptr<Error> shared = std::move(wrapped);
            errorConn_ = service_->scheduleTask([this, err = std::move(shared)] {
                onConnected(err->clone());
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
TCPStreamWin32::write(const ArrayView<std::uint8_t const>& data)
{
    POMDOG_ASSERT(isSocketValid(descriptor_));
    POMDOG_ASSERT(data.data() != nullptr);
    POMDOG_ASSERT(data.size() > 0);

    auto result = ::send(
        descriptor_,
        reinterpret_cast<const char*>(data.data()),
        static_cast<int>(data.size()),
        0);

    if (result == SOCKET_ERROR) {
        return errors::make("send failed with error: " + std::to_string(::WSAGetLastError()));
    }

    // NOTE: Update timestamp of last read/write
    lastActiveTime_ = service_->getNowTime();

    return nullptr;
}

bool TCPStreamWin32::isConnected() const noexcept
{
    return isConnected_;
}

void TCPStreamWin32::setTimeout(const Duration& timeoutIn)
{
    POMDOG_ASSERT(timeoutIn >= std::remove_reference_t<decltype(timeoutIn)>::zero());
    timeoutInterval_ = timeoutIn;
}

::SOCKET
TCPStreamWin32::getHandle() const noexcept
{
    return descriptor_;
}

void TCPStreamWin32::readEventLoop()
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

    const auto readSize = ::recv(descriptor_, reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), 0);
    if (readSize < 0) {
        const auto errorCode = ::WSAGetLastError();
        if (errorCode == WSAEWOULDBLOCK) {
            // NOTE: There is no data to be read yet
            return;
        }

        onRead({}, errors::make("read failed with error: " + std::to_string(errorCode)));
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
