// Copyright mogemimi. Distributed under the MIT license.

#include "executor.h"
#include "pomdog/chrono/game_clock.h"
#include "pomdog/network/array_view.h"
#include "pomdog/network/io_service.h"
#include "pomdog/network/udp_stream.h"
#include "pomdog/signals/connection_list.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_helper.h"
#include <catch_amalgamated.hpp>
#include <cstring>
#include <sstream>
#include <thread>
#include <unordered_map>

using namespace pomdog;

TEST_CASE("Ping Pong Server using UDP Connection", "[Network]")
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
            WARN("Unable to listen client");
            serverLogs.push_back(err->toString());
            executor.ExitLoop();
            return;
        }

        serverLogs.push_back("server connected");
    });
    conn += server.onReadFrom([&](const ArrayView<uint8_t>& view, const std::string_view& address, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            WARN("Unable to read message");
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
        auto buf = ArrayView<char const>{s.data(), s.size()}.viewAs<std::uint8_t const>();
        [[maybe_unused]] auto unused = server.writeTo(buf, address);

        server.disconnect();
        serverLogs.push_back("server disconnected");
    });

    auto [clientStream, clientErr] = UDPStream::connect(executor.GetService(), "localhost:30088");
    REQUIRE(clientErr == nullptr);
    auto client = std::move(clientStream);

    conn += client.onConnected([&](const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            WARN("Unable to connect server");
            clientLogs.push_back(err->toString());
            executor.ExitLoop();
            return;
        }

        clientLogs.push_back("client connected");
        std::string_view s = "ping";
        [[maybe_unused]] auto unused = client.write(ArrayView<char const>{s.data(), s.size()}.viewAs<std::uint8_t const>());
    });
    conn += client.onRead([&](const ArrayView<uint8_t>& view, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            WARN("Unable to read message");
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
