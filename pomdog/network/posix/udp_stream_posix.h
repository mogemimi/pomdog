// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/udp_stream.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <deque>
#include <expected>
#include <future>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// UDPStreamPOSIX is a UDP stream built on POSIX sockets.
class UDPStreamPOSIX final : public UDPStream {
public:
    UDPStreamPOSIX() noexcept;

    ~UDPStreamPOSIX() override;

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
    std::future<std::expected<int, std::unique_ptr<Error>>> connectResult_;
    std::deque<UDPStreamEvent> events_;

    /// The buffer that readSocket() reads into, allocated at the first read.
    std::vector<u8> receiveBuffer_;

    int descriptor_ = -1;
    bool isListening_ = false;
    bool isConnectedQueued_ = false;
    bool isClosed_ = false;
};

} // namespace pomdog::network::detail
