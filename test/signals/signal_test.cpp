// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/signals/connection.h"
#include "pomdog/signals/signal.h"
#include <catch_amalgamated.hpp>
#include <string>
#include <utility>
#include <vector>

using pomdog::Connection;
using pomdog::Signal;

TEST_CASE("Singal", "[Singals]")
{
    std::vector<std::string> messages;
    auto callback = [&](const std::string& s) { messages.push_back(s); };

    Signal<void(const std::string& s)> onText;
    REQUIRE(onText.getInvocationCount() == 0);

    SECTION("basic usage")
    {
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
    SECTION("when a new callback is assigned")
    {
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
        REQUIRE(onText.getInvocationCount() == 1);
        onText("start");
        REQUIRE(messages.size() == 5);
        REQUIRE(messages[0] == "start");
        REQUIRE(messages[1] == "count: 1");
        REQUIRE(messages[2] == "count: 2");
        REQUIRE(messages[3] == "count: 3");
        REQUIRE(messages[4] == "count: 4");
    }
    SECTION("Disconnect() was executed when calling a callback.")
    {
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
    SECTION("signal has expired before disconnecting connection")
    {
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
    SECTION("Disconnect() was executed when calling a callback.")
    {
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
    SECTION("Connect() was executed when calling a callback.")
    {
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
}

TEST_CASE("Singal and Slot", "[Singals]")
{
    SECTION("fundamental types - void")
    {
        Signal<void()> signal;
        std::string text;
        SECTION("void")
        {
            auto conn = signal.connect([&] { text = "Done"; });
            signal();
            REQUIRE(text == "Done");
            REQUIRE(conn.isConnected());
        }
        SECTION("void")
        {
            auto conn = signal.connect([&] { text += "Doki"; });
            signal();
            signal();
            signal();
            signal();
            REQUIRE(text == "DokiDokiDokiDoki");
            REQUIRE(conn.isConnected());
        }
        SECTION("void")
        {
            auto conn1 = signal.connect([&] { text += "Chuck "; });
            auto conn2 = signal.connect([&] { text += "Norris"; });
            signal();
            REQUIRE(text == "Chuck Norris");
            REQUIRE(conn1.isConnected());
            REQUIRE(conn2.isConnected());
        }
    }
    SECTION("fundamental types - bool")
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
    SECTION("fundamental types - int")
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
    SECTION("fundamental types - float")
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
    SECTION("fundamental types - char")
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
    SECTION("string")
    {
        Signal<void(const std::string&)> signal;
        std::string result;
        SECTION("string")
        {
            auto conn = signal.connect([&](const std::string& in) { result = in; });
            signal("Norris");
            REQUIRE(result == "Norris");
            signal(std::string{"Chuck"});
            REQUIRE(result == "Chuck");
            signal(result);
            REQUIRE(result == "Chuck");
            REQUIRE(conn.isConnected());
        }
        SECTION("string")
        {
            auto conn = signal.connect([&](std::string in) { result = in; });
            signal("Norris");
            REQUIRE(result == "Norris");
            signal(std::string{"Chuck"});
            REQUIRE(result == "Chuck");
            signal(result);
            REQUIRE(result == "Chuck");
            REQUIRE(conn.isConnected());
        }
    }
    SECTION("string")
    {
        Signal<void(std::string)> signal;
        std::string result;
        SECTION("string")
        {
            auto conn = signal.connect([&](std::string in) { result = in; });
            signal("Norris");
            REQUIRE(result == "Norris");
            signal(std::string{"Chuck"});
            REQUIRE(result == "Chuck");
            signal(result);
            REQUIRE(result == "Chuck");
            REQUIRE(conn.isConnected());
        }
        SECTION("string")
        {
            auto conn = signal.connect([&](const std::string& in) { result = in; });
            signal("Norris");
            REQUIRE(result == "Norris");
            signal(std::string{"Chuck"});
            REQUIRE(result == "Chuck");
            signal(result);
            REQUIRE(result == "Chuck");
            REQUIRE(conn.isConnected());
        }
    }
    SECTION("string")
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
    SECTION("custom class")
    {
        struct Chuck {
            int value;

            Chuck()
                : value(0)
            {
            }
            Chuck(int v)
                : value(v)
            {
            }
            Chuck(const Chuck&) = delete;
            Chuck& operator=(const Chuck&) = delete;
        };

        SECTION("custom class")
        {
            Signal<void(Chuck const&)> signal;
            int result = 0;
            auto conn1 = signal.connect([&](const Chuck& in) { result += in.value; });
            auto conn2 = signal.connect([&](const Chuck& in) { result += in.value; });
            Chuck const chuck{42};
            signal(chuck);
            REQUIRE(42 + 42 == result);
            REQUIRE(conn1.isConnected());
            REQUIRE(conn2.isConnected());
        }
        SECTION("custom class")
        {
            Signal<void(Chuck const&, Chuck const&)> signal;
            int result = 0;
            auto conn1 = signal.connect([&](const Chuck& in1, const Chuck& in2) { result += (in1.value + in2.value); });
            auto conn2 = signal.connect([&](const Chuck& in1, const Chuck& in2) { result += (in1.value + in2.value); });
            Chuck const chuck{3};
            signal(chuck, chuck);
            signal(chuck, chuck);
            signal(chuck, chuck);
            signal(chuck, chuck);
            REQUIRE(((3 + 3) * 2) * 4 == result);
            REQUIRE(conn1.isConnected());
            REQUIRE(conn2.isConnected());
        }
    }
    SECTION("invoke int")
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
    SECTION("disconnect")
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
    SECTION("move constructor")
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
    //SECTION("Move constructor reconnect")
    //{
    //    std::vector<int> integers;
    //    Signal<void(int)> signalOld;
    //    Signal<void(int)> signalNew = std::move(signalOld);
    //    auto slot = [&](int n) { integers.push_back(n); };
    //    auto connection = signalOld.connect(slot);
    //    signalNew(42);
    //    REQUIRE(integers.empty());
    //}
    SECTION("move assignment")
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
    SECTION("connect some slots")
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
    SECTION("disconnect slots")
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
