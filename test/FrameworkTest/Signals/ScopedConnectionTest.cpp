// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Signals/Connection.hpp>
#include <Pomdog/Signals/ScopedConnection.hpp>
#include <Pomdog/Signals/Signal.hpp>
#include <gtest/iutest_switch.hpp>
#include <utility>

using Pomdog::Connection;
using Pomdog::ScopedConnection;
using Pomdog::Signal;

TEST(ScopedConnection, ScopeGuard)
{
    Signal<void(int)> valueChanged;
    std::vector<int> integers;
    {
        ScopedConnection connection;
        auto slot = [&](int n){ integers.push_back(n); };
        connection = valueChanged.Connect(slot);

        valueChanged(42);
        valueChanged(43);
    }
    valueChanged(44);

    ASSERT_EQ(2, integers.size());
    EXPECT_EQ(42, integers[0]);
    EXPECT_EQ(43, integers[1]);
}

TEST(ScopedConnection, ExplicitDisconnect)
{
    Signal<void(int)> valueChanged;
    std::vector<int> integers;
    ScopedConnection connection;

    auto slot = [&](int n){ integers.push_back(n); };
    connection = valueChanged.Connect(slot);

    valueChanged(42);
    valueChanged(43);
    connection.Disconnect();
    valueChanged(44);

    ASSERT_EQ(2, integers.size());
    EXPECT_EQ(42, integers[0]);
    EXPECT_EQ(43, integers[1]);
}

TEST(ScopedConnection, MoveAssignment)
{
    Signal<void(int)> valueChanged;
    std::vector<int> integers;

    ScopedConnection connection1;
    {
        auto slot = [&](int n){ integers.push_back(n); };
        ScopedConnection connection2 = valueChanged.Connect(slot);
        valueChanged(42);
        connection1 = std::move(connection2);
        valueChanged(43);
    }

    valueChanged(44);
    connection1.Disconnect();
    valueChanged(45);

    ASSERT_EQ(3, integers.size());
    EXPECT_EQ(42, integers[0]);
    EXPECT_EQ(43, integers[1]);
    EXPECT_EQ(44, integers[2]);
}

TEST(ScopedConnection, CopyAssignmentEventConnection)
{
    Signal<void(int)> valueChanged;
    std::vector<int> integers;

    ScopedConnection connection1;
    {
        auto slot = [&](int n){ integers.push_back(n); };
        Connection connection2 = valueChanged.Connect(slot);
        valueChanged(42);
        connection1 = connection2;
        valueChanged(43);
    }

    valueChanged(44);
    connection1.Disconnect();
    valueChanged(45);

    ASSERT_EQ(3, integers.size());
    EXPECT_EQ(42, integers[0]);
    EXPECT_EQ(43, integers[1]);
    EXPECT_EQ(44, integers[2]);
}

TEST(ScopedConnection, MoveAssignmentEventConnection)
{
    Signal<void(int)> valueChanged;
    std::vector<int> integers;

    ScopedConnection connection1;
    {
        auto slot = [&](int n){ integers.push_back(n); };
        Connection connection2 = valueChanged.Connect(slot);

        valueChanged(42);
        connection1 = std::move(connection2);
        valueChanged(43);
        connection2.Disconnect();
        valueChanged(44);
    }

    valueChanged(45);
    connection1.Disconnect();
    valueChanged(46);

    ASSERT_EQ(4, integers.size());
    EXPECT_EQ(42, integers[0]);
    EXPECT_EQ(43, integers[1]);
    EXPECT_EQ(44, integers[2]);
    EXPECT_EQ(45, integers[3]);
}
