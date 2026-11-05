// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/connect_worker.h"
#include "pomdog/network/stream.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <chrono>
#include <deque>
#include <expected>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// TCPStreamPOSIX is a TCP stream built on POSIX sockets.
///
/// Only the blocking socket connection runs on a worker thread; `update()`
/// takes its result.
class TCPStreamPOSIX final : public Stream {
public:
    TCPStreamPOSIX() noexcept;

    ~TCPStreamPOSIX() override;

    /// Starts connecting to host on port.
    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    connect(std::string_view host, std::string_view port, const StreamOptions& options);

    void update() override;

    [[nodiscard]] std::optional<StreamEvent>
    pollEvent() override;

    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    write(std::span<const u8> data) override;

    void close() override;

    [[nodiscard]] bool
    isConnected() const noexcept override;

private:
    /// Closes the socket and queues a Disconnected event with err.
    void disconnect(std::unique_ptr<Error>&& err);

    void closeSocket() noexcept;

    /// Sends the bytes in sendBuffer_ until the socket takes no more.
    void flushSendBuffer();

    void readSocket();

private:
    ConnectWorker<int> connectWorker_;
    std::deque<StreamEvent> events_;

    /// The bytes that write() took and the socket has not sent yet.
    std::vector<u8> sendBuffer_;

    /// The buffer that readSocket() reads into, allocated at the first read.
    std::vector<u8> receiveBuffer_;

    int descriptor_ = -1;
    Duration idleTimeout_ = Duration::zero();
    std::chrono::steady_clock::time_point lastActiveTime_;
    bool isClosed_ = false;
};

} // namespace pomdog::network::detail
