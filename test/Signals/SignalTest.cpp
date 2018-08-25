// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Signals/Signal.hpp>
#include <gtest/iutest_switch.hpp>
#include <utility>
#include <vector>

using Pomdog::Signal;

TEST(Singal, Void)
{
    {
        Signal<void()> signal;
        std::string text;
        signal.Connect([&]{ text = "Done"; });
        signal();
        EXPECT_EQ("Done", text);
    }
    {
        Signal<void()> signal;
        std::string text;
        signal.Connect([&]{ text += "Doki"; });
        signal();
        signal();
        signal();
        signal();
        EXPECT_EQ("DokiDokiDokiDoki", text);
    }
    {
        Signal<void()> signal;
        std::string text;
        signal.Connect([&]{ text += "Chuck "; });
        signal.Connect([&]{ text += "Norris"; });
        signal();
        EXPECT_EQ("Chuck Norris", text);
    }
}

TEST(Singal, FundamentalTypes)
{
    {
        Signal<void(bool)> signal;
        bool result;
        signal.Connect([&](bool in){ result = in; });
        signal(true);
        EXPECT_EQ(true, result);
        signal(false);
        EXPECT_EQ(false, result);
        signal(result);
        EXPECT_EQ(false, result);
    }
    {
        Signal<void(int)> signal;
        int result;
        signal.Connect([&](int in){ result = in; });
        signal(42);
        EXPECT_EQ(42, result);
        signal(72);
        EXPECT_EQ(72, result);
        signal(result);
        EXPECT_EQ(72, result);
    }
    {
        Signal<void(float)> signal;
        float result;
        signal.Connect([&](float in){ result = in; });
        signal(42.0f);
        EXPECT_EQ(result, 42.0f);
        signal(72.0f);
        EXPECT_EQ(result, 72.0f);
        signal(result);
        EXPECT_EQ(result, 72.0f);
    }
    {
        Signal<void(char)> signal;
        char result;
        signal.Connect([&](char in){ result = in; });
        signal('a');
        EXPECT_EQ(result, 'a');
        signal('\n');
        EXPECT_EQ(result, '\n');
        signal(result);
        EXPECT_EQ(result, '\n');
    }
}

TEST(Singal, String)
{
    {
        Signal<void(std::string const&)> signal;
        std::string result;
        signal.Connect([&](std::string const& in){ result = in; });
        signal("Norris");
        EXPECT_EQ("Norris", result);
        signal(std::string{"Chuck"});
        EXPECT_EQ("Chuck", result);
        signal(result);
        EXPECT_EQ("Chuck", result);
    }
    {
        Signal<void(std::string const&)> signal;
        std::string result;
        signal.Connect([&](std::string in){ result = in; });
        signal("Norris");
        EXPECT_EQ("Norris", result);
        signal(std::string{"Chuck"});
        EXPECT_EQ("Chuck", result);
        signal(result);
        EXPECT_EQ("Chuck", result);
    }
    {
        Signal<void(std::string)> signal;
        std::string result;
        signal.Connect([&](std::string in){ result = in; });
        signal("Norris");
        EXPECT_EQ("Norris", result);
        signal(std::string{"Chuck"});
        EXPECT_EQ("Chuck", result);
        signal(result);
        EXPECT_EQ("Chuck", result);
    }
    {
        Signal<void(std::string)> signal;
        std::string result;
        signal.Connect([&](std::string const& in){ result = in; });
        signal("Norris");
        EXPECT_EQ("Norris", result);
        signal(std::string{"Chuck"});
        EXPECT_EQ("Chuck", result);
        signal(result);
        EXPECT_EQ("Chuck", result);
    }
    {
        Signal<void(std::string &)> signal;
        std::string result;
        signal.Connect([](std::string & in){ in += "Chuck "; });
        signal.Connect([](std::string & in){ in += "Norris"; });
        signal(result);
        EXPECT_EQ("Chuck Norris", result);
    }
}

TEST(Singal, CustomClass)
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
        EXPECT_EQ(42 + 42, result);
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
        EXPECT_EQ(((3 + 3) * 2) * 4, result);
    }
}

TEST(Singal, InvokeInt)
{
    Signal<void(int)> valueChanged;
    std::vector<int> integers;

    auto slot = [&](int n) { integers.push_back(n); };
    auto connection = valueChanged.Connect(slot);

    valueChanged(42);
    valueChanged(43);
    valueChanged(44);

    ASSERT_EQ(3, integers.size());
    EXPECT_EQ(42, integers[0]);
    EXPECT_EQ(43, integers[1]);
    EXPECT_EQ(44, integers[2]);
}

TEST(Singal, Disconnect)
{
    Signal<void(int)> valueChanged;
    std::vector<int> integers;

    auto slot = [&](int n) { integers.push_back(n); };
    auto connection = valueChanged.Connect(slot);

    valueChanged(42);
    valueChanged(43);
    connection.Disconnect();
    valueChanged(44);

    ASSERT_EQ(2, integers.size());
    EXPECT_EQ(42, integers[0]);
    EXPECT_EQ(43, integers[1]);
}

TEST(Singal, MoveConstructor)
{
    Signal<void(int)> valueChanged;
    std::vector<int> integers;

    auto slot = [&](int n) { integers.push_back(n); };
    auto connection = valueChanged.Connect(slot);

    Signal<void(int)> signalNew = std::move(valueChanged);

    signalNew(42);
    connection.Disconnect();
    signalNew(43);

    ASSERT_EQ(1, integers.size());
    EXPECT_EQ(42, integers[0]);
}

//TEST(Singal, MoveConstructor_Reconnect)
//{
//    std::vector<int> integers;
//    Signal<void(int)> signalOld;
//    Signal<void(int)> signalNew = std::move(signalOld);
//    auto slot = [&](int n) { integers.push_back(n); };
//    auto connection = signalOld.Connect(slot);
//    signalNew(42);
//    ASSERT_TRUE(integers.empty());
//}

TEST(Singal, MoveAssignment)
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

    ASSERT_EQ(3, integers.size());
    EXPECT_EQ(942, integers[0]);
    EXPECT_EQ(43, integers[1]);
    EXPECT_EQ(44, integers[2]);
}

TEST(Singal, ConnectSomeSlots)
{
    Signal<void(int)> valueChanged;
    std::vector<int> integers;

    auto slot = [&](int n) { integers.push_back(n); };
    valueChanged.Connect(slot);
    valueChanged.Connect(slot);
    valueChanged.Connect(slot);

    valueChanged(42);

    ASSERT_EQ(3, integers.size());
    EXPECT_EQ(42, integers[0]);
    EXPECT_EQ(42, integers[1]);
    EXPECT_EQ(42, integers[2]);
}

TEST(Singal, DisconnectSlots)
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

    ASSERT_EQ(6, integers.size());
    EXPECT_EQ(42, integers[0]);
    EXPECT_EQ(42, integers[1]);
    EXPECT_EQ(42, integers[2]);
    EXPECT_EQ(43, integers[3]);
    EXPECT_EQ(43, integers[4]);
    EXPECT_EQ(44, integers[5]);
}
