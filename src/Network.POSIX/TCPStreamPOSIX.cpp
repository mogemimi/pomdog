// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "TCPStreamPOSIX.hpp"
#include "SocketHelperPOSIX.hpp"
#include "../Utility/ErrorHelper.hpp"
#include "Pomdog/Network/ArrayView.hpp"
#include "Pomdog/Network/IOService.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <array>
#include <cstring>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {
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
    : service(serviceIn)
{
}

TCPStreamPOSIX::~TCPStreamPOSIX()
{
    if (blockingThread.joinable()) {
        blockingThread.join();
    }

    if (isSocketValid(this->descriptor)) {
        ::close(this->descriptor);
        this->descriptor = InvalidSocket;
    }
}

void TCPStreamPOSIX::Close()
{
    this->isConnected = false;
    this->eventLoopConn.Disconnect();
    this->errorConn.Disconnect();

    if (isSocketValid(this->descriptor)) {
        ::close(this->descriptor);
        this->descriptor = InvalidSocket;
    }
}

std::unique_ptr<Error>
TCPStreamPOSIX::Connect(std::string_view host, std::string_view port, const Duration& connectTimeout)
{
    POMDOG_ASSERT(service != nullptr);

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    const auto hostBuf = std::string{host};
    const auto portBuf = std::string{port};

    std::thread connectThread([this, hostBuf = std::move(hostBuf), portBuf = std::move(portBuf), connectTimeout = connectTimeout] {
        auto [fd, err] = Detail::ConnectSocketPOSIX(hostBuf, portBuf, SocketProtocol::TCP, connectTimeout);

        if (err != nullptr) {
            auto wrapped = Errors::Wrap(std::move(err), "couldn't connect to TCP socket on " + hostBuf + ":" + portBuf);
            std::shared_ptr<Error> shared = std::move(wrapped);
            errorConn = service->ScheduleTask([this, err = std::move(shared)] {
                this->OnConnected(err->Clone());
                this->errorConn.Disconnect();
            });
            return;
        }

        this->descriptor = fd;
        this->isConnected = true;

        eventLoopConn = service->ScheduleTask([this] {
            // NOTE: Update timestamp of last connection
            this->lastActiveTime = this->service->GetNowTime();

            this->OnConnected(nullptr);
            eventLoopConn.Disconnect();
            eventLoopConn = service->ScheduleTask([this] { this->ReadEventLoop(); });
        });
    });

    this->blockingThread = std::move(connectThread);

    return nullptr;
}

std::unique_ptr<Error>
TCPStreamPOSIX::Write(const ArrayView<std::uint8_t const>& data)
{
    POMDOG_ASSERT(isSocketValid(descriptor));
    POMDOG_ASSERT(data.GetData() != nullptr);
    POMDOG_ASSERT(data.GetSize() > 0);

    auto result = ::send(this->descriptor, data.GetData(), data.GetSize(), flags);

    if (result == -1) {
        auto errorCode = Detail::ToErrc(errno);
        return Errors::New(errorCode, "write failed with error");
    }

    // NOTE: Update timestamp of last read/write
    this->lastActiveTime = service->GetNowTime();

    return nullptr;
}

bool TCPStreamPOSIX::IsConnected() const noexcept
{
    return this->isConnected;
}

void TCPStreamPOSIX::SetTimeout(const Duration& timeoutIn)
{
    POMDOG_ASSERT(timeoutIn >= std::remove_reference_t<decltype(timeoutIn)>::zero());
    this->timeoutInterval = timeoutIn;
}

int TCPStreamPOSIX::GetHandle() const noexcept
{
    return this->descriptor;
}

void TCPStreamPOSIX::ReadEventLoop()
{
    POMDOG_ASSERT(isSocketValid(descriptor));

    if (timeoutInterval.has_value()) {
        if ((service->GetNowTime() - lastActiveTime) > *timeoutInterval) {
            this->OnRead({}, Errors::New("timeout socket connection"));
            this->Close();
            this->OnDisconnect();
            return;
        }
    }

    // NOTE: Read per 1 frame (= 1/60 seconds) for a packet up to 2048 bytes.
    std::array<std::uint8_t, 2048> buffer;

    ssize_t readSize = ::recv(this->descriptor, buffer.data(), buffer.size(), flags);
    if (readSize < 0) {
        const auto errorCode = Detail::ToErrc(errno);
        if (errorCode == std::errc::resource_unavailable_try_again || errorCode == std::errc::operation_would_block) {
            // NOTE: There is no data to be read yet
            return;
        }

        this->OnRead({}, Errors::New(errorCode, "read failed with error"));
        return;
    }

    // NOTE: Update timestamp of last read/write
    this->lastActiveTime = service->GetNowTime();

    POMDOG_ASSERT(readSize >= 0);
    auto view = ArrayView<std::uint8_t>{buffer.data(), static_cast<std::size_t>(readSize)};
    this->OnRead(std::move(view), nullptr);

    if (readSize == 0) {
        // NOTE: When the peer socket has performed orderly shutdown,
        // the read size will be 0 (meaning the "end-of-file").
        this->Close();
        this->OnDisconnect();
    }
}

} // namespace Pomdog::Detail
