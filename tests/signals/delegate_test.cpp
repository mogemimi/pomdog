// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/signals/connection.h"
#include "pomdog/signals/delegate.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Delegate;

TEST_CASE("Delegate")
{
    SUBCASE("connect and invoke")
    {
        Delegate<void(int)> delegate;
        int result = 0;

        [[maybe_unused]] auto conn = delegate.connect([&result](int x) {
            result = x;
        });

        delegate(42);
        REQUIRE(result == 42);
    }
    SUBCASE("reconnect")
    {
        Delegate<void(int)> delegate;
        int result = 0;

        [[maybe_unused]] auto conn1 = delegate.connect([&result](int x) {
            result = x;
        });

        delegate(42);
        REQUIRE(result == 42);

        [[maybe_unused]] auto conn2 = delegate.connect([&result](int x) {
            result = x * 2;
        });

        delegate(42);
        REQUIRE(result == 84);
    }
    SUBCASE("disconnect")
    {
        Delegate<void(int)> delegate;
        int result = 0;

        [[maybe_unused]] auto conn = delegate.connect([&result](int x) {
            result = x;
        });

        delegate(42);
        REQUIRE(result == 42);

        delegate.disconnect();

        result = 0;
        delegate(100);
        REQUIRE(result == 0);
    }
    SUBCASE("multiple arguments")
    {
        Delegate<void(int, float, std::string)> delegate;
        int intResult = 0;
        float floatResult = 0.0f;
        std::string stringResult;

        [[maybe_unused]] auto conn = delegate.connect([&](int i, float f, std::string s) {
            intResult = i;
            floatResult = f;
            stringResult = s;
        });

        delegate(42, 3.14f, "hello");
        REQUIRE(intResult == 42);
        REQUIRE(floatResult == doctest::Approx(3.14).epsilon(0.001));
        REQUIRE(stringResult == "hello");
    }
    SUBCASE("no arguments")
    {
        Delegate<void()> delegate;
        int counter = 0;

        [[maybe_unused]] auto conn = delegate.connect([&counter]() {
            ++counter;
        });

        delegate();
        REQUIRE(counter == 1);

        delegate();
        REQUIRE(counter == 2);
    }
    SUBCASE("isConnected")
    {
        Delegate<void()> delegate;

        REQUIRE_FALSE(delegate.isConnected());

        [[maybe_unused]] auto conn = delegate.connect([]() {});
        REQUIRE(delegate.isConnected());

        delegate.disconnect();
        REQUIRE_FALSE(delegate.isConnected());
    }
    SUBCASE("bool operator")
    {
        std::vector<std::string> messages;
        Delegate<void(const std::string& s)> onText;

        [[maybe_unused]] auto conn = onText.connect([&](const std::string& s) { messages.push_back(s); });
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
    SUBCASE("when called recursively")
    {
        std::vector<std::string> messages;
        Delegate<void(const std::string& s)> onText;

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
    SUBCASE("when a nullptr is assigned")
    {
        Delegate<void(const std::string& s)> onText;
        auto conn = onText.connect(nullptr);
        REQUIRE(!conn.isConnected());
        REQUIRE(!onText.isConnected());

        onText("hi");

        conn.disconnect();
        REQUIRE(!conn.isConnected());
        REQUIRE(!onText.isConnected());
    }
    SUBCASE("Disconnect in callback")
    {
        std::vector<std::string> messages;
        Delegate<void(const std::string& s)> onText;
        pomdog::Connection conn;
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
    SUBCASE("Connect in callback")
    {
        std::vector<std::string> messages;
        Delegate<void(const std::string& s)> onText;
        pomdog::Connection conn;
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
    SUBCASE("signal has expired before disconnecting connection")
    {
        std::vector<std::string> messages;
        auto callback = [&](const std::string& s) { messages.push_back(s); };

        Delegate<void(const std::string& s)> onText;
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
