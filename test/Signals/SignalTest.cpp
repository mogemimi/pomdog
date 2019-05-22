// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Signals/Signal.hpp"
#include "catch.hpp"
#include <utility>
#include <vector>

using Pomdog::Signal;

TEST_CASE("Singal", "[Singal]")
{
    SECTION("Void")
    {
        {
            Signal<void()> signal;
            std::string text;
            signal.Connect([&]{ text = "Done"; });
            signal();
            REQUIRE(text == "Done");
        }
        {
            Signal<void()> signal;
            std::string text;
            signal.Connect([&]{ text += "Doki"; });
            signal();
            signal();
            signal();
            signal();
            REQUIRE(text == "DokiDokiDokiDoki");
        }
        {
            Signal<void()> signal;
            std::string text;
            signal.Connect([&]{ text += "Chuck "; });
            signal.Connect([&]{ text += "Norris"; });
            signal();
            REQUIRE(text == "Chuck Norris");
        }
    }
    SECTION("FundamentalTypes")
    {
        {
            Signal<void(bool)> signal;
            bool result;
            signal.Connect([&](bool in){ result = in; });
            signal(true);
            REQUIRE(true == result);
            signal(false);
            REQUIRE(false == result);
            signal(result);
            REQUIRE(false == result);
        }
        {
            Signal<void(int)> signal;
            int result;
            signal.Connect([&](int in){ result = in; });
            signal(42);
            REQUIRE(result == 42);
            signal(72);
            REQUIRE(result == 72);
            signal(result);
            REQUIRE(result == 72);
        }
        {
            Signal<void(float)> signal;
            float result;
            signal.Connect([&](float in){ result = in; });
            signal(42.0f);
            REQUIRE(result == 42.0f);
            signal(72.0f);
            REQUIRE(result == 72.0f);
            signal(result);
            REQUIRE(result == 72.0f);
        }
        {
            Signal<void(char)> signal;
            char result;
            signal.Connect([&](char in){ result = in; });
            signal('a');
            REQUIRE(result == 'a');
            signal('\n');
            REQUIRE(result == '\n');
            signal(result);
            REQUIRE(result == '\n');
        }
    }
    SECTION("String")
    {
        {
            Signal<void(std::string const&)> signal;
            std::string result;
            signal.Connect([&](std::string const& in){ result = in; });
            signal("Norris");
            REQUIRE(result == "Norris");
            signal(std::string{"Chuck"});
            REQUIRE(result == "Chuck");
            signal(result);
            REQUIRE(result == "Chuck");
        }
        {
            Signal<void(std::string const&)> signal;
            std::string result;
            signal.Connect([&](std::string in){ result = in; });
            signal("Norris");
            REQUIRE(result == "Norris");
            signal(std::string{"Chuck"});
            REQUIRE(result == "Chuck");
            signal(result);
            REQUIRE(result == "Chuck");
        }
        {
            Signal<void(std::string)> signal;
            std::string result;
            signal.Connect([&](std::string in){ result = in; });
            signal("Norris");
            REQUIRE(result == "Norris");
            signal(std::string{"Chuck"});
            REQUIRE(result == "Chuck");
            signal(result);
            REQUIRE(result == "Chuck");
        }
        {
            Signal<void(std::string)> signal;
            std::string result;
            signal.Connect([&](std::string const& in){ result = in; });
            signal("Norris");
            REQUIRE(result == "Norris");
            signal(std::string{"Chuck"});
            REQUIRE(result == "Chuck");
            signal(result);
            REQUIRE(result == "Chuck");
        }
        {
            Signal<void(std::string &)> signal;
            std::string result;
            signal.Connect([](std::string & in){ in += "Chuck "; });
            signal.Connect([](std::string & in){ in += "Norris"; });
            signal(result);
            REQUIRE(result == "Chuck Norris");
        }
    }
    SECTION("CustomClass")
    {
        struct Chuck {
            int value;

            Chuck(): value(0) {}
            Chuck(int v): value(v) {}

            Chuck(Chuck const&) = delete;
            Chuck(Chuck &&) = delete;
            Chuck & operator=(Chuck const&) = delete;
            Chuck & operator=(Chuck &&) = delete;
        };

        {
            Signal<void(Chuck const&)> signal;
            int result = 0;
            signal.Connect([&](Chuck const& in){ result += in.value; });
            signal.Connect([&](Chuck const& in){ result += in.value; });
            Chuck const chuck{42};
            signal(chuck);
            REQUIRE(42 + 42 == result);
        }
        {
            Signal<void(Chuck const&, Chuck const&)> signal;
            int result = 0;
            signal.Connect([&](Chuck const& in1, Chuck const& in2){ result += (in1.value + in2.value); });
            signal.Connect([&](Chuck const& in1, Chuck const& in2){ result += (in1.value + in2.value); });
            Chuck const chuck{3};
            signal(chuck, chuck);
            signal(chuck, chuck);
            signal(chuck, chuck);
            signal(chuck, chuck);
            REQUIRE(((3 + 3) * 2) * 4 == result);
        }
    }
    SECTION("InvokeInt")
    {
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
    SECTION("Disconnect")
    {
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
    SECTION("Move constructor")
    {
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
    SECTION("MoveAssignment")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        auto slot = [&](int n) { integers.push_back(n); };
        auto connection = valueChanged.Connect(slot);

        Signal<void(int)> signalNew;
        signalNew.Connect([&](int n) {
            integers.push_back(n + 900);
        });

        signalNew(42);
        valueChanged(43);
        signalNew = std::move(valueChanged);
        signalNew(44);
        connection.Disconnect();
        signalNew(45);

        REQUIRE(integers.size() == 3);
        REQUIRE(integers[0] == 942);
        REQUIRE(integers[1] == 43);
        REQUIRE(integers[2] == 44);
    }
    SECTION("Connect some slots")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        auto slot = [&](int n) { integers.push_back(n); };
        valueChanged.Connect(slot);
        valueChanged.Connect(slot);
        valueChanged.Connect(slot);

        valueChanged(42);

        REQUIRE(integers.size() == 3);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 42);
        REQUIRE(integers[2] == 42);
    }
    SECTION("Disconnect slots")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        auto slot = [&](int n) { integers.push_back(n); };
        auto connection1 = valueChanged.Connect(slot);
        auto connection2 = valueChanged.Connect(slot);
        auto connection3 = valueChanged.Connect(slot);

        valueChanged(42);
        connection1.Disconnect();
        valueChanged(43);
        connection2.Disconnect();
        valueChanged(44);
        connection3.Disconnect();
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
