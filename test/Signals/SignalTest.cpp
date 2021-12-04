// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Signals/Signal.hpp"
#include "Pomdog/Signals/Connection.hpp"
#include "catch_amalgamated.hpp"
#include <string>
#include <utility>
#include <vector>

using Pomdog::Connection;
using Pomdog::Signal;

TEST_CASE("Singal", "[Singals]")
{
    std::vector<std::string> messages;
    auto callback = [&](const std::string& s) { messages.push_back(s); };

    Signal<void(const std::string& s)> onText;
    REQUIRE(onText.GetInvocationCount() == 0);

    SECTION("basic usage") {
        auto conn = onText.Connect(callback);
        REQUIRE(conn.IsConnected());
        REQUIRE(onText.GetInvocationCount() == 1);

        onText("hi");
        REQUIRE(messages.size() == 1);
        REQUIRE(messages.back() == "hi");
        onText("hello");
        REQUIRE(messages.size() == 2);
        REQUIRE(messages.back() == "hello");

        // NOTE: connection has expired
        conn.Disconnect();
        REQUIRE(!conn.IsConnected());
        REQUIRE(onText.GetInvocationCount() == 0);

        onText("ok");
        REQUIRE(messages.size() == 2);
        REQUIRE(messages[0] == "hi");
        REQUIRE(messages[1] == "hello");
    }
    SECTION("when a new callback is assigned") {
        auto conn1 = onText.Connect(callback);
        REQUIRE(conn1.IsConnected());
        REQUIRE(onText.GetInvocationCount() == 1);

        onText("hi");
        REQUIRE(messages.size() == 1);
        REQUIRE(messages.back() == "hi");

        auto conn2 = onText.Connect([&](const std::string& s) { messages.push_back(">> " + s); });
        REQUIRE(conn1.IsConnected());
        REQUIRE(conn2.IsConnected());
        REQUIRE(onText.GetInvocationCount() == 2);

        onText("hello");
        REQUIRE(messages.size() == 3);
        REQUIRE(messages[1] == "hello");
        REQUIRE(messages[2] == ">> hello");

        conn1.Disconnect();
        REQUIRE(!conn1.IsConnected());
        REQUIRE(conn2.IsConnected());
        REQUIRE(onText.GetInvocationCount() == 1);
        conn2.Disconnect();
        REQUIRE(!conn2.IsConnected());
        REQUIRE(onText.GetInvocationCount() == 0);

        onText("ok");
        REQUIRE(messages.size() == 3);
        REQUIRE(messages[0] == "hi");
        REQUIRE(messages[1] == "hello");
        REQUIRE(messages[2] == ">> hello");
    }
    SECTION("when called recursively") {
        int i = 0;
        auto conn = onText.Connect([&](const std::string& s) {
            if (i > 4) {
                return;
            }
            messages.push_back(s);
            ++i;
            onText("count: " + std::to_string(i));
        });
        REQUIRE(onText.GetInvocationCount() == 1);
        onText("start");
        REQUIRE(messages.size() == 5);
        REQUIRE(messages[0] == "start");
        REQUIRE(messages[1] == "count: 1");
        REQUIRE(messages[2] == "count: 2");
        REQUIRE(messages[3] == "count: 3");
        REQUIRE(messages[4] == "count: 4");
    }
    SECTION("Disconnect() was executed when calling a callback.") {
        Connection conn;
        conn = onText.Connect([&](const std::string& s) {
            messages.push_back(s);
            REQUIRE(conn.IsConnected());
            REQUIRE(onText.GetInvocationCount() == 1);

            conn.Disconnect();
            REQUIRE(!conn.IsConnected());
            REQUIRE(onText.GetInvocationCount() == 0);
        });
        REQUIRE(conn.IsConnected());
        REQUIRE(onText.GetInvocationCount() == 1);
        onText("hi");
        REQUIRE(!conn.IsConnected());
        REQUIRE(onText.GetInvocationCount() == 0);
        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0] == "hi");
    }
    SECTION("signal has expired before disconnecting connection") {
        auto conn = onText.Connect(callback);
        REQUIRE(conn.IsConnected());
        REQUIRE(onText.GetInvocationCount() == 1);

        onText = decltype(onText){};
        REQUIRE(!conn.IsConnected());
        REQUIRE(onText.GetInvocationCount() == 0);

        onText("hi");
        REQUIRE(messages.empty());

        conn.Disconnect();
        REQUIRE(!conn.IsConnected());
        REQUIRE(onText.GetInvocationCount() == 0);
    }
    SECTION("Disconnect() was executed when calling a callback.") {
        Connection conn;
        conn = onText.Connect([&](const std::string& s) {
            messages.push_back(s);
            REQUIRE(conn.IsConnected());
            REQUIRE(onText.GetInvocationCount() == 1);

            conn.Disconnect();
            REQUIRE(!conn.IsConnected());
            REQUIRE(onText.GetInvocationCount() == 0);
        });
        REQUIRE(conn.IsConnected());
        REQUIRE(onText.GetInvocationCount() == 1);
        onText("hi");
        REQUIRE(!conn.IsConnected());
        REQUIRE(onText.GetInvocationCount() == 0);
        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0] == "hi");
    }
    SECTION("Connect() was executed when calling a callback.") {
        Connection conn;
        conn = onText.Connect([&](const std::string& s) {
            messages.push_back(">> " + s);
            REQUIRE(conn.IsConnected());

            auto conn2 = onText.Connect(callback);
            REQUIRE(conn.IsConnected());
            REQUIRE(conn2.IsConnected());
        });
        REQUIRE(conn.IsConnected());
        REQUIRE(onText.GetInvocationCount() == 1);

        onText("hi");
        REQUIRE(conn.IsConnected());
        REQUIRE(onText.GetInvocationCount() == 2);
        REQUIRE(messages.size() == 1);
        REQUIRE(messages[0] == ">> hi");

        onText("hello");
        REQUIRE(conn.IsConnected());
        REQUIRE(onText.GetInvocationCount() == 3);

        REQUIRE(messages.size() == 3);
        REQUIRE(messages[0] == ">> hi");
        REQUIRE(messages[1] == ">> hello");
        REQUIRE(messages[2] == "hello");
    }
}

TEST_CASE("Singal and Slot", "[Singals]")
{
    SECTION("fundamental types - void") {
        Signal<void()> signal;
        std::string text;
        SECTION("void") {
            auto conn = signal.Connect([&]{ text = "Done"; });
            signal();
            REQUIRE(text == "Done");
            REQUIRE(conn.IsConnected());
        }
        SECTION("void") {
            auto conn = signal.Connect([&]{ text += "Doki"; });
            signal();
            signal();
            signal();
            signal();
            REQUIRE(text == "DokiDokiDokiDoki");
            REQUIRE(conn.IsConnected());
        }
        SECTION("void") {
            auto conn1 = signal.Connect([&]{ text += "Chuck "; });
            auto conn2 = signal.Connect([&]{ text += "Norris"; });
            signal();
            REQUIRE(text == "Chuck Norris");
            REQUIRE(conn1.IsConnected());
            REQUIRE(conn2.IsConnected());
        }
    }
    SECTION("fundamental types - bool") {
        Signal<void(bool)> signal;
        bool result;
        auto conn = signal.Connect([&](bool in) { result = in; });
        signal(true);
        REQUIRE(true == result);
        signal(false);
        REQUIRE(false == result);
        signal(result);
        REQUIRE(false == result);
        REQUIRE(conn.IsConnected());
    }
    SECTION("fundamental types - int") {
        Signal<void(int)> signal;
        int result;
        auto conn = signal.Connect([&](int in) { result = in; });
        signal(42);
        REQUIRE(result == 42);
        signal(72);
        REQUIRE(result == 72);
        signal(result);
        REQUIRE(result == 72);
        REQUIRE(conn.IsConnected());
    }
    SECTION("fundamental types - float") {
        Signal<void(float)> signal;
        float result;
        auto conn = signal.Connect([&](float in) { result = in; });
        signal(42.0f);
        REQUIRE(result == 42.0f);
        signal(72.0f);
        REQUIRE(result == 72.0f);
        signal(result);
        REQUIRE(result == 72.0f);
        REQUIRE(conn.IsConnected());
    }
    SECTION("fundamental types - char") {
        Signal<void(char)> signal;
        char result;
        auto conn = signal.Connect([&](char in) { result = in; });
        signal('a');
        REQUIRE(result == 'a');
        signal('\n');
        REQUIRE(result == '\n');
        signal(result);
        REQUIRE(result == '\n');
        REQUIRE(conn.IsConnected());
    }
    SECTION("string") {
        Signal<void(const std::string&)> signal;
        std::string result;
        SECTION("string") {
            auto conn = signal.Connect([&](const std::string& in){ result = in; });
            signal("Norris");
            REQUIRE(result == "Norris");
            signal(std::string{"Chuck"});
            REQUIRE(result == "Chuck");
            signal(result);
            REQUIRE(result == "Chuck");
            REQUIRE(conn.IsConnected());
        }
        SECTION("string") {
            auto conn = signal.Connect([&](std::string in){ result = in; });
            signal("Norris");
            REQUIRE(result == "Norris");
            signal(std::string{"Chuck"});
            REQUIRE(result == "Chuck");
            signal(result);
            REQUIRE(result == "Chuck");
            REQUIRE(conn.IsConnected());
        }
    }
    SECTION("string") {
        Signal<void(std::string)> signal;
        std::string result;
        SECTION("string") {
            auto conn = signal.Connect([&](std::string in){ result = in; });
            signal("Norris");
            REQUIRE(result == "Norris");
            signal(std::string{"Chuck"});
            REQUIRE(result == "Chuck");
            signal(result);
            REQUIRE(result == "Chuck");
            REQUIRE(conn.IsConnected());
        }
        SECTION("string") {
            auto conn = signal.Connect([&](const std::string& in){ result = in; });
            signal("Norris");
            REQUIRE(result == "Norris");
            signal(std::string{"Chuck"});
            REQUIRE(result == "Chuck");
            signal(result);
            REQUIRE(result == "Chuck");
            REQUIRE(conn.IsConnected());
        }
    }
    SECTION("string") {
        Signal<void(std::string&)> signal;
        std::string result;
        auto conn1 = signal.Connect([](std::string& in) { in += "Chuck "; });
        auto conn2 = signal.Connect([](std::string& in) { in += "Norris"; });
        signal(result);
        REQUIRE(result == "Chuck Norris");
        REQUIRE(conn1.IsConnected());
        REQUIRE(conn2.IsConnected());
    }
    SECTION("custom class") {
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

        SECTION("custom class") {
            Signal<void(Chuck const&)> signal;
            int result = 0;
            auto conn1 = signal.Connect([&](const Chuck& in) { result += in.value; });
            auto conn2 = signal.Connect([&](const Chuck& in) { result += in.value; });
            Chuck const chuck{42};
            signal(chuck);
            REQUIRE(42 + 42 == result);
            REQUIRE(conn1.IsConnected());
            REQUIRE(conn2.IsConnected());
        }
        SECTION("custom class") {
            Signal<void(Chuck const&, Chuck const&)> signal;
            int result = 0;
            auto conn1 = signal.Connect([&](const Chuck& in1, const Chuck& in2) { result += (in1.value + in2.value); });
            auto conn2 = signal.Connect([&](const Chuck& in1, const Chuck& in2) { result += (in1.value + in2.value); });
            Chuck const chuck{3};
            signal(chuck, chuck);
            signal(chuck, chuck);
            signal(chuck, chuck);
            signal(chuck, chuck);
            REQUIRE(((3 + 3) * 2) * 4 == result);
            REQUIRE(conn1.IsConnected());
            REQUIRE(conn2.IsConnected());
        }
    }
    SECTION("invoke int") {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        auto slot = [&](int n) { integers.push_back(n); };
        auto connection = valueChanged.Connect(slot);

        valueChanged(42);
        valueChanged(43);
        valueChanged(44);

        REQUIRE(integers.size() == 3);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
        REQUIRE(integers[2] == 44);
    }
    SECTION("disconnect") {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        auto slot = [&](int n) { integers.push_back(n); };
        auto connection = valueChanged.Connect(slot);

        valueChanged(42);
        valueChanged(43);
        connection.Disconnect();
        valueChanged(44);

        REQUIRE(integers.size() == 2);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
    }
    SECTION("move constructor") {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        auto slot = [&](int n) { integers.push_back(n); };
        auto connection = valueChanged.Connect(slot);

        Signal<void(int)> signalNew = std::move(valueChanged);

        signalNew(42);
        connection.Disconnect();
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
    //    auto connection = signalOld.Connect(slot);
    //    signalNew(42);
    //    REQUIRE(integers.empty());
    //}
    SECTION("move assignment") {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        auto slot = [&](int n) { integers.push_back(n); };
        auto conn1 = valueChanged.Connect(slot);

        Signal<void(int)> signalNew;
        auto conn2 = signalNew.Connect([&](int n) {
            integers.push_back(n + 900);
        });
        REQUIRE(conn2.IsConnected());

        signalNew(42);
        valueChanged(43);
        signalNew = std::move(valueChanged);
        REQUIRE(conn1.IsConnected());
        REQUIRE(!conn2.IsConnected());

        signalNew(44);
        conn1.Disconnect();
        REQUIRE(!conn1.IsConnected());
        REQUIRE(!conn2.IsConnected());
        signalNew(45);

        REQUIRE(integers.size() == 3);
        REQUIRE(integers[0] == 942);
        REQUIRE(integers[1] == 43);
        REQUIRE(integers[2] == 44);
    }
    SECTION("connect some slots") {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        auto slot = [&](int n) { integers.push_back(n); };
        auto conn1 = valueChanged.Connect(slot);
        auto conn2 = valueChanged.Connect(slot);
        auto conn3 = valueChanged.Connect(slot);

        REQUIRE(conn1.IsConnected());
        REQUIRE(conn2.IsConnected());
        REQUIRE(conn3.IsConnected());

        valueChanged(42);

        REQUIRE(integers.size() == 3);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 42);
        REQUIRE(integers[2] == 42);
    }
    SECTION("disconnect slots") {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        auto slot = [&](int n) { integers.push_back(n); };
        auto conn1 = valueChanged.Connect(slot);
        auto conn2 = valueChanged.Connect(slot);
        auto conn3 = valueChanged.Connect(slot);
        REQUIRE(conn1.IsConnected());
        REQUIRE(conn2.IsConnected());
        REQUIRE(conn3.IsConnected());

        valueChanged(42);
        conn1.Disconnect();
        REQUIRE(!conn1.IsConnected());
        REQUIRE(conn2.IsConnected());
        REQUIRE(conn3.IsConnected());

        valueChanged(43);
        conn2.Disconnect();
        REQUIRE(!conn1.IsConnected());
        REQUIRE(!conn2.IsConnected());
        REQUIRE(conn3.IsConnected());

        valueChanged(44);
        conn3.Disconnect();
        REQUIRE(!conn1.IsConnected());
        REQUIRE(!conn2.IsConnected());
        REQUIRE(!conn3.IsConnected());

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
