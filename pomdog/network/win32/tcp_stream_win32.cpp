// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/network/win32/tcp_stream_win32.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/network/array_view.hpp"
#include "pomdog/network/io_service.hpp"
#include "pomdog/network/win32/socket_helper_win32.hpp"
#include "pomdog/utility/assert.hpp"
#include "pomdog/utility/error_helper.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {
namespace {

bool isSocketValid(::SOCKET descriptor) noexcept
{
    return descriptor != INVALID_SOCKET;
}

} // namespace

TCPStreamWin32::TCPStreamWin32(IOService* serviceIn)
    : service(serviceIn)
{
}

TCPStreamWin32::~TCPStreamWin32()
{
    if (blockingThread.joinable()) {
        blockingThread.join();
    }

    if (isSocketValid(this->descriptor)) {
        ::closesocket(this->descriptor);
        this->descriptor = INVALID_SOCKET;
    }
}

void TCPStreamWin32::Close()
{
    this->isConnected = false;
    this->eventLoopConn.Disconnect();
    this->errorConn.Disconnect();

    if (isSocketValid(this->descriptor)) {
        ::closesocket(this->descriptor);
        this->descriptor = INVALID_SOCKET;
    }
}

std::unique_ptr<Error>
TCPStreamWin32::Connect(std::string_view host, std::string_view port, const Duration& connectTimeout)
{
    POMDOG_ASSERT(service != nullptr);

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    const auto hostBuf = std::string{host};
    const auto portBuf = std::string{port};

    std::thread connectThread([this, hostBuf = std::move(hostBuf), portBuf = std::move(portBuf), connectTimeout = connectTimeout] {
        auto [fd, err] = Detail::ConnectSocketWin32(hostBuf, portBuf, SocketProtocol::TCP, connectTimeout);

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
TCPStreamWin32::Write(const ArrayView<std::uint8_t const>& data)
{
    POMDOG_ASSERT(isSocketValid(descriptor));
    POMDOG_ASSERT(data.GetData() != nullptr);
    POMDOG_ASSERT(data.GetSize() > 0);

    auto result = ::send(
        this->descriptor,
        reinterpret_cast<const char*>(data.GetData()),
        static_cast<int>(data.GetSize()),
        0);

    if (result == SOCKET_ERROR) {
        return Errors::New("send failed with error: " + std::to_string(::WSAGetLastError()));
    }

    // NOTE: Update timestamp of last read/write
    this->lastActiveTime = service->GetNowTime();

    return nullptr;
}

bool TCPStreamWin32::IsConnected() const noexcept
{
    return this->isConnected;
}

void TCPStreamWin32::SetTimeout(const Duration& timeoutIn)
{
    POMDOG_ASSERT(timeoutIn >= std::remove_reference_t<decltype(timeoutIn)>::zero());
    this->timeoutInterval = timeoutIn;
}

::SOCKET TCPStreamWin32::GetHandle() const noexcept
{
    return this->descriptor;
}

void TCPStreamWin32::ReadEventLoop()
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

    const auto readSize = ::recv(this->descriptor, reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), 0);
    if (readSize < 0) {
        const auto errorCode = ::WSAGetLastError();
        if (errorCode == WSAEWOULDBLOCK) {
            // NOTE: There is no data to be read yet
            return;
        }

        this->OnRead({}, Errors::New("read failed with error: " + std::to_string(errorCode)));
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
