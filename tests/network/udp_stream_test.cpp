// Copyright mogemimi. Distributed under the MIT license.

#include "executor.h"
#include "tests/testing/testing.h"
#include "pomdog/chrono/game_clock.h"
#include "pomdog/network/io_service.h"
#include "pomdog/network/udp_stream.h"
#include "pomdog/signals/connection_list.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <cstring>
#include <sstream>
#include <thread>
#include <unordered_map>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using namespace pomdog;

TEST_CASE("Ping Pong Server using UDP Connection")
{
    Executor executor;
    ConnectionList conn;

    std::vector<std::string> serverLogs;
    std::vector<std::string> clientLogs;

    auto [serverStream, serverErr] = UDPStream::listen(executor.GetService(), "localhost:30088");
    REQUIRE(serverErr == nullptr);
    auto server = std::move(serverStream);

    conn += server.onConnected([&](const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            INFO("unable to listen client:", err->toString());
            serverLogs.push_back(err->toString());
            executor.ExitLoop();
            return;
        }

        serverLogs.push_back("server connected");
    });
    conn += server.onReadFrom([&](std::span<uint8_t> view, const std::string_view& address, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            INFO("unable to read message:", err->toString());
            serverLogs.push_back(err->toString());
            executor.ExitLoop();
            return;
        }

        serverLogs.push_back("server read");
        auto text = std::string_view{reinterpret_cast<const char*>(view.data()), view.size()};

        if (text != "ping") {
            executor.ExitLoop();
        }
        REQUIRE(text == "ping");
        REQUIRE(!address.empty());

        std::string_view s = "pong";
        auto buf = std::span<const std::uint8_t>{
            reinterpret_cast<const std::uint8_t*>(s.data()),
            s.size()};
        [[maybe_unused]] auto unused = server.writeTo(buf, address);

        server.disconnect();
        serverLogs.push_back("server disconnected");
    });

    auto [clientStream, clientErr] = UDPStream::connect(executor.GetService(), "localhost:30088");
    REQUIRE(clientErr == nullptr);
    auto client = std::move(clientStream);

    conn += client.onConnected([&](const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            INFO("unable to connect server:", err->toString());
            clientLogs.push_back(err->toString());
            executor.ExitLoop();
            return;
        }

        clientLogs.push_back("client connected");
        std::string_view s = "ping";
        [[maybe_unused]] auto unused = client.write(std::span<const std::uint8_t>{
            reinterpret_cast<const std::uint8_t*>(s.data()),
            s.size()});
    });
    conn += client.onRead([&](std::span<uint8_t> view, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            INFO("unable to read message:", err->toString());
            clientLogs.push_back(err->toString());
            executor.ExitLoop();
            return;
        }

        clientLogs.push_back("client read");
        auto text = std::string_view{reinterpret_cast<const char*>(view.data()), view.size()};

        if (text != "pong") {
            executor.ExitLoop();
        }
        REQUIRE(text == "pong");

        client.disconnect();
        clientLogs.push_back("client disconnected");
        executor.ExitLoop();
    });

    executor.RunLoop();

    REQUIRE(serverLogs.size() == 3);
    REQUIRE(serverLogs[0] == "server connected");
    REQUIRE(serverLogs[1] == "server read");
    REQUIRE(serverLogs[2] == "server disconnected");

    REQUIRE(clientLogs.size() == 3);
    REQUIRE(clientLogs[0] == "client connected");
    REQUIRE(clientLogs[1] == "client read");
    REQUIRE(clientLogs[2] == "client disconnected");
}
