// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/udp_stream.h"
#include "pomdog/network/win32/winsock_win32.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <WinSock2.h>
#include <deque>
#include <expected>
#include <future>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// UDPStreamWin32 is a UDP stream built on Winsock.
class UDPStreamWin32 final : public UDPStream {
public:
    UDPStreamWin32() noexcept;

    ~UDPStreamWin32() override;

    /// Starts connecting to host on port. Resolving the host runs on a worker
    /// thread.
    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    connect(std::string_view host, std::string_view port);

    /// Binds to host on port to receive datagrams from any sender.
    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    listen(std::string_view host, std::string_view port);

    void update() override;

    [[nodiscard]] std::optional<UDPStreamEvent>
    pollEvent() override;

    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    write(std::span<const u8> data) override;

    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    writeTo(std::span<const u8> data, std::string_view address) override;

    void close() override;

private:
    /// Closes the socket and queues a Disconnected event with err.
    void disconnect(std::unique_ptr<Error>&& err);

    void closeSocket() noexcept;

    void readSocket();

private:
    WinsockLibrary winsock_;
    std::future<std::expected<::SOCKET, std::unique_ptr<Error>>> connectResult_;
    std::deque<UDPStreamEvent> events_;
    ::SOCKET descriptor_ = INVALID_SOCKET;
    bool isListening_ = false;
    bool isConnectedQueued_ = false;
    bool isClosed_ = false;
};

} // namespace pomdog::network::detail
