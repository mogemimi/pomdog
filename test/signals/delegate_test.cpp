// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/signals/connection.h"
#include "pomdog/signals/delegate.h"
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
    REQUIRE(!onText.isConnected());

    SECTION("basic usage")
    {
        auto conn = onText.connect(callback);
        REQUIRE(conn.isConnected());
        REQUIRE(onText.isConnected());

        onText("hi");
        REQUIRE(messages.size() == 1);
        REQUIRE(messages.back() == "hi");
        onText("hello");
        REQUIRE(messages.size() == 2);
        REQUIRE(messages.back() == "hello");

        // NOTE: connection has expired
        conn.disconnect();
        REQUIRE(!conn.isConnected());
        REQUIRE(!onText.isConnected());

        onText("ok");
        REQUIRE(messages.size() == 2);
        REQUIRE(messages[0] == "hi");
        REQUIRE(messages[1] == "hello");
    }
    SECTION("bool operator")
    {
        auto conn = onText.connect(callback);
        REQUIRE(conn.isConnected());
        REQUIRE(onText.isConnected());

        if (onText.isConnected()) {
            onText("hi");
        }
        REQUIRE(messages.size() == 1);
        REQUIRE(messages.back() == "hi");

        // NOTE: connection has expired
        conn.disconnect();
        REQUIRE(!conn.isConnected());
        REQUIRE(!onText.isConnected());

        onText("ok");
        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0] == "hi");
    }
    SECTION("when a new callback is assigned")
    {
        auto conn1 = onText.connect(callback);
        REQUIRE(conn1.isConnected());
        REQUIRE(onText.isConnected());

        onText("hi");
        REQUIRE(messages.size() == 1);
        REQUIRE(messages.back() == "hi");

        // NOTE: conn1 has expired
        auto conn2 = onText.connect([&](const std::string& s) { messages.push_back(">> " + s); });
        REQUIRE(!conn1.isConnected());
        REQUIRE(conn2.isConnected());
        REQUIRE(onText.isConnected());

        onText("hello");
        REQUIRE(messages.size() == 2);
        REQUIRE(messages.back() == ">> hello");

        conn1.disconnect();
        REQUIRE(onText.isConnected());
        conn2.disconnect();
        REQUIRE(!conn2.isConnected());
        REQUIRE(!onText.isConnected());

        onText("ok");
        REQUIRE(messages.size() == 2);
        REQUIRE(messages[0] == "hi");
        REQUIRE(messages[1] == ">> hello");
    }
    SECTION("when called recursively")
    {
        int i = 0;
        auto conn = onText.connect([&](const std::string& s) {
            if (i > 4) {
                return;
            }
            messages.push_back(s);
            ++i;
            onText("count: " + std::to_string(i));
        });
        REQUIRE(onText.isConnected());
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
        auto conn = onText.connect(nullptr);
        REQUIRE(!conn.isConnected());
        REQUIRE(!onText.isConnected());

        onText("hi");
        REQUIRE(messages.empty());

        conn.disconnect();
        REQUIRE(!conn.isConnected());
        REQUIRE(!onText.isConnected());
    }
    SECTION("Disconnect() was executed when calling a callback.")
    {
        Connection conn;
        conn = onText.connect([&](const std::string& s) {
            messages.push_back(s);
            REQUIRE(conn.isConnected());
            REQUIRE(onText.isConnected());

            conn.disconnect();
            REQUIRE(!conn.isConnected());
            REQUIRE(!onText.isConnected());
        });
        REQUIRE(conn.isConnected());
        REQUIRE(onText.isConnected());
        onText("hi");
        REQUIRE(!conn.isConnected());
        REQUIRE(!onText.isConnected());
        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0] == "hi");
    }
    SECTION("Connect() was executed when calling a callback.")
    {
        Connection conn;
        conn = onText.connect([&](const std::string& s) {
            messages.push_back(s);
            REQUIRE(conn.isConnected());
            REQUIRE(onText.isConnected());

            auto conn2 = onText.connect(nullptr);
            REQUIRE(!conn.isConnected());
            REQUIRE(!conn2.isConnected());
            REQUIRE(!onText.isConnected());
        });
        REQUIRE(conn.isConnected());
        REQUIRE(onText.isConnected());
        onText("hi");
        REQUIRE(!conn.isConnected());
        REQUIRE(!onText.isConnected());
        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0] == "hi");
    }
    SECTION("signal has expired before disconnecting connection")
    {
        auto conn = onText.connect(callback);
        REQUIRE(conn.isConnected());
        REQUIRE(onText.isConnected());

        onText = decltype(onText){};
        REQUIRE(!conn.isConnected());
        REQUIRE(!onText.isConnected());

        onText("hi");
        REQUIRE(messages.empty());

        conn.disconnect();
        REQUIRE(!conn.isConnected());
        REQUIRE(!onText.isConnected());
    }
}
