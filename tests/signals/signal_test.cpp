// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/signals/connection.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Connection;
using pomdog::Signal;

TEST_CASE("Signal")
{
    SUBCASE("connect and invoke")
    {
        Signal<void(int)> signal;
        int result = 0;

        auto connection = signal.connect([&result](int x) {
            result = x;
        });

        signal(42);
        REQUIRE(result == 42);
    }
    SUBCASE("invoke multiple slots")
    {
        Signal<void(int)> signal;
        int result1 = 0;
        int result2 = 0;

        auto connection1 = signal.connect([&result1](int x) {
            result1 = x;
        });
        auto connection2 = signal.connect([&result2](int x) {
            result2 = x * 2;
        });

        signal(42);
        REQUIRE(result1 == 42);
        REQUIRE(result2 == 84);
    }
    SUBCASE("disconnect")
    {
        Signal<void(int)> signal;
        int result = 0;

        auto connection = signal.connect([&result](int x) {
            result = x;
        });

        signal(42);
        REQUIRE(result == 42);

        connection.disconnect();

        signal(100);
        REQUIRE(result == 42);
    }
    SUBCASE("multiple arguments")
    {
        Signal<void(int, float, std::string)> signal;
        int intResult = 0;
        float floatResult = 0.0f;
        std::string stringResult;

        auto connection = signal.connect([&](int i, float f, std::string s) {
            intResult = i;
            floatResult = f;
            stringResult = s;
        });

        signal(42, 3.14f, "hello");
        REQUIRE(intResult == 42);
        REQUIRE(floatResult == doctest::Approx(3.14).epsilon(0.001));
        REQUIRE(stringResult == "hello");
    }
    SUBCASE("no arguments")
    {
        Signal<void()> signal;
        int counter = 0;

        auto connection = signal.connect([&counter]() {
            ++counter;
        });

        signal();
        REQUIRE(counter == 1);

        signal();
        REQUIRE(counter == 2);
    }
    SUBCASE("connection scope")
    {
        // NOTE: Connection does NOT disconnect on destruction (not RAII).
        // Use ScopedConnection for RAII behavior.
        Signal<void(int)> signal;
        int result = 0;

        {
            auto connection = signal.connect([&result](int x) {
                result = x;
            });

            signal(42);
            REQUIRE(result == 42);
        }

        // Connection destroyed but slot is still connected
        signal(100);
        REQUIRE(result == 100);
    }
    SUBCASE("disconnect during invoke")
    {
        Signal<void()> signal;
        int counter = 0;
        pomdog::Connection connection;

        connection = signal.connect([&]() {
            ++counter;
            connection.disconnect();
        });

        signal();
        REQUIRE(counter == 1);

        signal();
        REQUIRE(counter == 1);
    }
    SUBCASE("basic usage with getInvocationCount")
    {
        std::vector<std::string> messages;
        auto callback = [&](const std::string& s) { messages.push_back(s); };

        Signal<void(const std::string& s)> onText;
        REQUIRE(onText.getInvocationCount() == 0);

        auto conn = onText.connect(callback);
        REQUIRE(conn.isConnected());
        REQUIRE(onText.getInvocationCount() == 1);

        onText("hi");
        REQUIRE(messages.size() == 1);
        REQUIRE(messages.back() == "hi");
        onText("hello");
        REQUIRE(messages.size() == 2);
        REQUIRE(messages.back() == "hello");

        // NOTE: connection has expired
        conn.disconnect();
        REQUIRE(!conn.isConnected());
        REQUIRE(onText.getInvocationCount() == 0);

        onText("ok");
        REQUIRE(messages.size() == 2);
        REQUIRE(messages[0] == "hi");
        REQUIRE(messages[1] == "hello");
    }
    SUBCASE("when a new callback is assigned")
    {
        std::vector<std::string> messages;
        auto callback = [&](const std::string& s) { messages.push_back(s); };

        Signal<void(const std::string& s)> onText;
        REQUIRE(onText.getInvocationCount() == 0);

        auto conn1 = onText.connect(callback);
        REQUIRE(conn1.isConnected());
        REQUIRE(onText.getInvocationCount() == 1);

        onText("hi");
        REQUIRE(messages.size() == 1);
        REQUIRE(messages.back() == "hi");

        auto conn2 = onText.connect([&](const std::string& s) { messages.push_back(">> " + s); });
        REQUIRE(conn1.isConnected());
        REQUIRE(conn2.isConnected());
        REQUIRE(onText.getInvocationCount() == 2);

        onText("hello");
        REQUIRE(messages.size() == 3);
        REQUIRE(messages[1] == "hello");
        REQUIRE(messages[2] == ">> hello");

        conn1.disconnect();
        REQUIRE(!conn1.isConnected());
        REQUIRE(conn2.isConnected());
        REQUIRE(onText.getInvocationCount() == 1);
        conn2.disconnect();
        REQUIRE(!conn2.isConnected());
        REQUIRE(onText.getInvocationCount() == 0);

        onText("ok");
        REQUIRE(messages.size() == 3);
        REQUIRE(messages[0] == "hi");
        REQUIRE(messages[1] == "hello");
        REQUIRE(messages[2] == ">> hello");
    }
    SUBCASE("when called recursively")
    {
        std::vector<std::string> messages;
        Signal<void(const std::string& s)> onText;

        int i = 0;
        auto conn = onText.connect([&](const std::string& s) {
            if (i > 4) {
                return;
            }
            messages.push_back(s);
            ++i;
            onText("count: " + std::to_string(i));
        });
        REQUIRE(onText.getInvocationCount() == 1);
        onText("start");
        REQUIRE(messages.size() == 5);
        REQUIRE(messages[0] == "start");
        REQUIRE(messages[1] == "count: 1");
        REQUIRE(messages[2] == "count: 2");
        REQUIRE(messages[3] == "count: 3");
        REQUIRE(messages[4] == "count: 4");
    }
    SUBCASE("disconnect in callback")
    {
        std::vector<std::string> messages;
        Signal<void(const std::string& s)> onText;

        Connection conn;
        conn = onText.connect([&](const std::string& s) {
            messages.push_back(s);
            REQUIRE(conn.isConnected());
            REQUIRE(onText.getInvocationCount() == 1);

            conn.disconnect();
            REQUIRE(!conn.isConnected());
            REQUIRE(onText.getInvocationCount() == 0);
        });
        REQUIRE(conn.isConnected());
        REQUIRE(onText.getInvocationCount() == 1);
        onText("hi");
        REQUIRE(!conn.isConnected());
        REQUIRE(onText.getInvocationCount() == 0);
        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0] == "hi");
    }
    SUBCASE("signal has expired before disconnecting connection")
    {
        std::vector<std::string> messages;
        auto callback = [&](const std::string& s) { messages.push_back(s); };

        Signal<void(const std::string& s)> onText;
        auto conn = onText.connect(callback);
        REQUIRE(conn.isConnected());
        REQUIRE(onText.getInvocationCount() == 1);

        onText = decltype(onText){};
        REQUIRE(!conn.isConnected());
        REQUIRE(onText.getInvocationCount() == 0);

        onText("hi");
        REQUIRE(messages.empty());

        conn.disconnect();
        REQUIRE(!conn.isConnected());
        REQUIRE(onText.getInvocationCount() == 0);
    }
    SUBCASE("connect in callback")
    {
        std::vector<std::string> messages;
        auto callback = [&](const std::string& s) { messages.push_back(s); };

        Signal<void(const std::string& s)> onText;
        Connection conn;
        conn = onText.connect([&](const std::string& s) {
            messages.push_back(">> " + s);
            REQUIRE(conn.isConnected());

            auto conn2 = onText.connect(callback);
            REQUIRE(conn.isConnected());
            REQUIRE(conn2.isConnected());
        });
        REQUIRE(conn.isConnected());
        REQUIRE(onText.getInvocationCount() == 1);

        onText("hi");
        REQUIRE(conn.isConnected());
        REQUIRE(onText.getInvocationCount() == 2);
        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0] == ">> hi");

        onText("hello");
        REQUIRE(conn.isConnected());
        REQUIRE(onText.getInvocationCount() == 3);

        REQUIRE(messages.size() == 3);
        REQUIRE(messages[0] == ">> hi");
        REQUIRE(messages[1] == ">> hello");
        REQUIRE(messages[2] == "hello");
    }
    SUBCASE("fundamental types - bool")
    {
        Signal<void(bool)> signal;
        bool result;
        auto conn = signal.connect([&](bool in) { result = in; });
        signal(true);
        REQUIRE(true == result);
        signal(false);
        REQUIRE(false == result);
        signal(result);
        REQUIRE(false == result);
        REQUIRE(conn.isConnected());
    }
    SUBCASE("fundamental types - int")
    {
        Signal<void(int)> signal;
        int result;
        auto conn = signal.connect([&](int in) { result = in; });
        signal(42);
        REQUIRE(result == 42);
        signal(72);
        REQUIRE(result == 72);
        signal(result);
        REQUIRE(result == 72);
        REQUIRE(conn.isConnected());
    }
    SUBCASE("fundamental types - float")
    {
        Signal<void(float)> signal;
        float result;
        auto conn = signal.connect([&](float in) { result = in; });
        signal(42.0f);
        REQUIRE(result == 42.0f);
        signal(72.0f);
        REQUIRE(result == 72.0f);
        signal(result);
        REQUIRE(result == 72.0f);
        REQUIRE(conn.isConnected());
    }
    SUBCASE("fundamental types - char")
    {
        Signal<void(char)> signal;
        char result;
        auto conn = signal.connect([&](char in) { result = in; });
        signal('a');
        REQUIRE(result == 'a');
        signal('\n');
        REQUIRE(result == '\n');
        signal(result);
        REQUIRE(result == '\n');
        REQUIRE(conn.isConnected());
    }
    SUBCASE("string by const ref")
    {
        Signal<void(const std::string&)> signal;
        std::string result;
        auto conn = signal.connect([&](const std::string& in) { result = in; });
        signal("Norris");
        REQUIRE(result == "Norris");
        signal(std::string{"Chuck"});
        REQUIRE(result == "Chuck");
        signal(result);
        REQUIRE(result == "Chuck");
        REQUIRE(conn.isConnected());
    }
    SUBCASE("string by value")
    {
        Signal<void(const std::string&)> signal;
        std::string result;
        auto conn = signal.connect([&](std::string in) { result = in; });
        signal("Norris");
        REQUIRE(result == "Norris");
        signal(std::string{"Chuck"});
        REQUIRE(result == "Chuck");
        signal(result);
        REQUIRE(result == "Chuck");
        REQUIRE(conn.isConnected());
    }
    SUBCASE("string by mutable ref")
    {
        Signal<void(std::string&)> signal;
        std::string result;
        auto conn1 = signal.connect([](std::string& in) { in += "Chuck "; });
        auto conn2 = signal.connect([](std::string& in) { in += "Norris"; });
        signal(result);
        REQUIRE(result == "Chuck Norris");
        REQUIRE(conn1.isConnected());
        REQUIRE(conn2.isConnected());
    }
    SUBCASE("invoke int")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        auto slot = [&](int n) { integers.push_back(n); };
        auto connection = valueChanged.connect(slot);

        valueChanged(42);
        valueChanged(43);
        valueChanged(44);

        REQUIRE(integers.size() == 3);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
        REQUIRE(integers[2] == 44);
    }
    SUBCASE("disconnect after multiple invocations")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        auto slot = [&](int n) { integers.push_back(n); };
        auto connection = valueChanged.connect(slot);

        valueChanged(42);
        valueChanged(43);
        connection.disconnect();
        valueChanged(44);

        REQUIRE(integers.size() == 2);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
    }
    SUBCASE("move constructor")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        auto slot = [&](int n) { integers.push_back(n); };
        auto connection = valueChanged.connect(slot);

        Signal<void(int)> signalNew = std::move(valueChanged);

        signalNew(42);
        connection.disconnect();
        signalNew(43);

        REQUIRE(integers.size() == 1);
        REQUIRE(integers[0] == 42);
    }
    SUBCASE("move assignment")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        auto slot = [&](int n) { integers.push_back(n); };
        auto conn1 = valueChanged.connect(slot);

        Signal<void(int)> signalNew;
        auto conn2 = signalNew.connect([&](int n) {
            integers.push_back(n + 900);
        });
        REQUIRE(conn2.isConnected());

        signalNew(42);
        valueChanged(43);
        signalNew = std::move(valueChanged);
        REQUIRE(conn1.isConnected());
        REQUIRE(!conn2.isConnected());

        signalNew(44);
        conn1.disconnect();
        REQUIRE(!conn1.isConnected());
        REQUIRE(!conn2.isConnected());
        signalNew(45);

        REQUIRE(integers.size() == 3);
        REQUIRE(integers[0] == 942);
        REQUIRE(integers[1] == 43);
        REQUIRE(integers[2] == 44);
    }
    SUBCASE("connect some slots")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        auto slot = [&](int n) { integers.push_back(n); };
        auto conn1 = valueChanged.connect(slot);
        auto conn2 = valueChanged.connect(slot);
        auto conn3 = valueChanged.connect(slot);

        REQUIRE(conn1.isConnected());
        REQUIRE(conn2.isConnected());
        REQUIRE(conn3.isConnected());

        valueChanged(42);

        REQUIRE(integers.size() == 3);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 42);
        REQUIRE(integers[2] == 42);
    }
    SUBCASE("disconnect slots progressively")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        auto slot = [&](int n) { integers.push_back(n); };
        auto conn1 = valueChanged.connect(slot);
        auto conn2 = valueChanged.connect(slot);
        auto conn3 = valueChanged.connect(slot);
        REQUIRE(conn1.isConnected());
        REQUIRE(conn2.isConnected());
        REQUIRE(conn3.isConnected());

        valueChanged(42);
        conn1.disconnect();
        REQUIRE(!conn1.isConnected());
        REQUIRE(conn2.isConnected());
        REQUIRE(conn3.isConnected());

        valueChanged(43);
        conn2.disconnect();
        REQUIRE(!conn1.isConnected());
        REQUIRE(!conn2.isConnected());
        REQUIRE(conn3.isConnected());

        valueChanged(44);
        conn3.disconnect();
        REQUIRE(!conn1.isConnected());
        REQUIRE(!conn2.isConnected());
        REQUIRE(!conn3.isConnected());

        valueChanged(45);

        REQUIRE(integers.size() == 6);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 42);
        REQUIRE(integers[2] == 42);
        REQUIRE(integers[3] == 43);
        REQUIRE(integers[4] == 43);
        REQUIRE(integers[5] == 44);
    }
}
