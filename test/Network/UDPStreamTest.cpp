// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/network/udp_stream.hpp"
#include "Executor.hpp"
#include "pomdog/application/game_clock.hpp"
#include "pomdog/network/array_view.hpp"
#include "pomdog/network/io_service.hpp"
#include "pomdog/signals/connection_list.hpp"
#include "pomdog/utility/errors.hpp"
#include "pomdog/utility/string_helper.hpp"
#include "catch_amalgamated.hpp"
#include <cstring>
#include <sstream>
#include <thread>
#include <unordered_map>

using namespace Pomdog;

TEST_CASE("Ping Pong Server using UDP Connection", "[Network]")
{
    Executor executor;
    ConnectionList conn;

    std::vector<std::string> serverLogs;
    std::vector<std::string> clientLogs;

    auto [serverStream, serverErr] = UDPStream::Listen(executor.GetService(), "localhost:30088");
    REQUIRE(serverErr == nullptr);
    auto server = std::move(serverStream);

    conn += server.OnConnected([&](const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            WARN("Unable to listen client");
            serverLogs.push_back(err->ToString());
            executor.ExitLoop();
            return;
        }

        serverLogs.push_back("server connected");
    });
    conn += server.OnReadFrom([&](const ArrayView<uint8_t>& view, const std::string_view& address, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            WARN("Unable to read message");
            serverLogs.push_back(err->ToString());
            executor.ExitLoop();
            return;
        }

        serverLogs.push_back("server read");
        auto text = std::string_view{reinterpret_cast<const char*>(view.GetData()), view.GetSize()};

        if (text != "ping") {
            executor.ExitLoop();
        }
        REQUIRE(text == "ping");
        REQUIRE(!address.empty());

        std::string_view s = "pong";
        auto buf = ArrayView<char const>{s.data(), s.size()}.ViewAs<std::uint8_t const>();
        server.WriteTo(buf, address);

        server.Disconnect();
        serverLogs.push_back("server disconnected");
    });

    auto [clientStream, clientErr] = UDPStream::Connect(executor.GetService(), "localhost:30088");
    REQUIRE(clientErr == nullptr);
    auto client = std::move(clientStream);

    conn += client.OnConnected([&](const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            WARN("Unable to connect server");
            clientLogs.push_back(err->ToString());
            executor.ExitLoop();
            return;
        }

        clientLogs.push_back("client connected");
        std::string_view s = "ping";
        client.Write(ArrayView<char const>{s.data(), s.size()}.ViewAs<std::uint8_t const>());
    });
    conn += client.OnRead([&](const ArrayView<uint8_t>& view, const std::unique_ptr<Error>& err) {
        if (err != nullptr) {
            WARN("Unable to read message");
            clientLogs.push_back(err->ToString());
            executor.ExitLoop();
            return;
        }

        clientLogs.push_back("client read");
        auto text = std::string_view{reinterpret_cast<const char*>(view.GetData()), view.GetSize()};

        if (text != "pong") {
            executor.ExitLoop();
        }
        REQUIRE(text == "pong");

        client.Disconnect();
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
