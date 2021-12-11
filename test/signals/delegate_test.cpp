// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/signals/connection.hpp"
#include "pomdog/signals/delegate.hpp"
#include <catch_amalgamated.hpp>
#include <string>
#include <utility>
#include <vector>

using pomdog::Connection;
using pomdog::Delegate;

TEST_CASE("Delegate", "[Signals]")
{
    std::vector<std::string> messages;
    auto callback = [&](const std::string& s) { messages.push_back(s); };

    Delegate<void(const std::string& s)> onText;
    REQUIRE(!onText.IsConnected());

    SECTION("basic usage")
    {
        auto conn = onText.Connect(callback);
        REQUIRE(conn.IsConnected());
        REQUIRE(onText.IsConnected());

        onText("hi");
        REQUIRE(messages.size() == 1);
        REQUIRE(messages.back() == "hi");
        onText("hello");
        REQUIRE(messages.size() == 2);
        REQUIRE(messages.back() == "hello");

        // NOTE: connection has expired
        conn.Disconnect();
        REQUIRE(!conn.IsConnected());
        REQUIRE(!onText.IsConnected());

        onText("ok");
        REQUIRE(messages.size() == 2);
        REQUIRE(messages[0] == "hi");
        REQUIRE(messages[1] == "hello");
    }
    SECTION("bool operator")
    {
        auto conn = onText.Connect(callback);
        REQUIRE(conn.IsConnected());
        REQUIRE(onText.IsConnected());
        REQUIRE(onText);

        if (onText) {
            onText("hi");
        }
        REQUIRE(messages.size() == 1);
        REQUIRE(messages.back() == "hi");

        // NOTE: connection has expired
        conn.Disconnect();
        REQUIRE(!conn.IsConnected());
        REQUIRE(!onText.IsConnected());
        REQUIRE(!onText);

        onText("ok");
        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0] == "hi");
    }
    SECTION("when a new callback is assigned")
    {
        auto conn1 = onText.Connect(callback);
        REQUIRE(conn1.IsConnected());
        REQUIRE(onText.IsConnected());

        onText("hi");
        REQUIRE(messages.size() == 1);
        REQUIRE(messages.back() == "hi");

        // NOTE: conn1 has expired
        auto conn2 = onText.Connect([&](const std::string& s) { messages.push_back(">> " + s); });
        REQUIRE(!conn1.IsConnected());
        REQUIRE(conn2.IsConnected());
        REQUIRE(onText.IsConnected());

        onText("hello");
        REQUIRE(messages.size() == 2);
        REQUIRE(messages.back() == ">> hello");

        conn1.Disconnect();
        REQUIRE(onText.IsConnected());
        conn2.Disconnect();
        REQUIRE(!conn2.IsConnected());
        REQUIRE(!onText.IsConnected());

        onText("ok");
        REQUIRE(messages.size() == 2);
        REQUIRE(messages[0] == "hi");
        REQUIRE(messages[1] == ">> hello");
    }
    SECTION("when called recursively")
    {
        int i = 0;
        auto conn = onText.Connect([&](const std::string& s) {
            if (i > 4) {
                return;
            }
            messages.push_back(s);
            ++i;
            onText("count: " + std::to_string(i));
        });
        REQUIRE(onText.IsConnected());
        onText("start");
        REQUIRE(messages.size() == 5);
        REQUIRE(messages[0] == "start");
        REQUIRE(messages[1] == "count: 1");
        REQUIRE(messages[2] == "count: 2");
        REQUIRE(messages[3] == "count: 3");
        REQUIRE(messages[4] == "count: 4");
    }
    SECTION("when a nullptr is assigned")
    {
        auto conn = onText.Connect(nullptr);
        REQUIRE(!conn.IsConnected());
        REQUIRE(!onText.IsConnected());

        onText("hi");
        REQUIRE(messages.empty());

        conn.Disconnect();
        REQUIRE(!conn.IsConnected());
        REQUIRE(!onText.IsConnected());
    }
    SECTION("Disconnect() was executed when calling a callback.")
    {
        Connection conn;
        conn = onText.Connect([&](const std::string& s) {
            messages.push_back(s);
            REQUIRE(conn.IsConnected());
            REQUIRE(onText.IsConnected());

            conn.Disconnect();
            REQUIRE(!conn.IsConnected());
            REQUIRE(!onText.IsConnected());
        });
        REQUIRE(conn.IsConnected());
        REQUIRE(onText.IsConnected());
        onText("hi");
        REQUIRE(!conn.IsConnected());
        REQUIRE(!onText.IsConnected());
        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0] == "hi");
    }
    SECTION("Connect() was executed when calling a callback.")
    {
        Connection conn;
        conn = onText.Connect([&](const std::string& s) {
            messages.push_back(s);
            REQUIRE(conn.IsConnected());
            REQUIRE(onText.IsConnected());

            auto conn2 = onText.Connect(nullptr);
            REQUIRE(!conn.IsConnected());
            REQUIRE(!conn2.IsConnected());
            REQUIRE(!onText.IsConnected());
        });
        REQUIRE(conn.IsConnected());
        REQUIRE(onText.IsConnected());
        onText("hi");
        REQUIRE(!conn.IsConnected());
        REQUIRE(!onText.IsConnected());
        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0] == "hi");
    }
    SECTION("signal has expired before disconnecting connection")
    {
        auto conn = onText.Connect(callback);
        REQUIRE(conn.IsConnected());
        REQUIRE(onText.IsConnected());

        onText = decltype(onText){};
        REQUIRE(!conn.IsConnected());
        REQUIRE(!onText.IsConnected());

        onText("hi");
        REQUIRE(messages.empty());

        conn.Disconnect();
        REQUIRE(!conn.IsConnected());
        REQUIRE(!onText.IsConnected());
    }
}
