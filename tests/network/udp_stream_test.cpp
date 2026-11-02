// Copyright mogemimi. Distributed under the MIT license.

#include "network_test_helper.h"
#include "tests/testing/testing.h"
#include "pomdog/network/udp_stream.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using namespace pomdog;
using namespace pomdog::network;

namespace {

[[nodiscard]] std::span<const u8>
toBytes(std::string_view text) noexcept
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    return std::span<const u8>{reinterpret_cast<const u8*>(text.data()), text.size()};
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

[[nodiscard]] std::string_view
toText(const std::vector<u8>& bytes) noexcept
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    return std::string_view{reinterpret_cast<const char*>(bytes.data()), bytes.size()};
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

} // namespace

TEST_CASE("Ping Pong Server using UDP Connection")
{
    std::vector<std::string> serverLogs;
    std::vector<std::string> clientLogs;

    auto serverResult = listenUDPStream("localhost:30088");
    REQUIRE(serverResult.has_value());
    auto server = std::move(*serverResult);

    auto clientResult = connectUDPStream("localhost:30088");
    REQUIRE(clientResult.has_value());
    auto client = std::move(*clientResult);

    const auto finished = runUntil([&] {
        server->update();
        while (auto event = server->pollEvent()) {
            if (event->kind == StreamEventKind::Connected) {
                serverLogs.push_back("server connected");
            }
            else if (event->kind == StreamEventKind::Received) {
                serverLogs.push_back("server read");
                REQUIRE(toText(event->data) == "ping");
                REQUIRE(!event->address.empty());

                REQUIRE(server->writeTo(toBytes("pong"), event->address).has_value());

                server->close();
                serverLogs.push_back("server disconnected");
            }
            else {
                REQUIRE(event->kind == StreamEventKind::Disconnected);
                REQUIRE(event->error != nullptr);
                reportNetworkError(*event->error);
                return true;
            }
        }

        client->update();
        while (auto event = client->pollEvent()) {
            if (event->kind == StreamEventKind::Connected) {
                clientLogs.push_back("client connected");
                REQUIRE(client->write(toBytes("ping")).has_value());
            }
            else if (event->kind == StreamEventKind::Received) {
                clientLogs.push_back("client read");
                REQUIRE(toText(event->data) == "pong");
                REQUIRE(event->address.empty());

                client->close();
                clientLogs.push_back("client disconnected");
                return true;
            }
            else {
                REQUIRE(event->kind == StreamEventKind::Disconnected);
                REQUIRE(event->error != nullptr);
                reportNetworkError(*event->error);
                return true;
            }
        }
        return false;
    });
    REQUIRE(finished);

    REQUIRE(serverLogs.size() == 3);
    REQUIRE(serverLogs[0] == "server connected");
    REQUIRE(serverLogs[1] == "server read");
    REQUIRE(serverLogs[2] == "server disconnected");

    REQUIRE(clientLogs.size() == 3);
    REQUIRE(clientLogs[0] == "client connected");
    REQUIRE(clientLogs[1] == "client read");
    REQUIRE(clientLogs[2] == "client disconnected");

    // NOTE: A closed stream reports no events and cannot send.
    server->update();
    REQUIRE_FALSE(server->pollEvent().has_value());
    REQUIRE_FALSE(server->writeTo(toBytes("pong"), "localhost:30088").has_value());
}

TEST_CASE("UDP stream fails to listen on an address in use")
{
    auto first = listenUDPStream("localhost:30089");
    REQUIRE(first.has_value());

    auto second = listenUDPStream("localhost:30089");
    REQUIRE_FALSE(second.has_value());
    REQUIRE(second.error() != nullptr);
}
