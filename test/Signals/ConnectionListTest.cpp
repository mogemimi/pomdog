// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Signals/ConnectionList.hpp>
#include <Pomdog/Signals/Signal.hpp>
#include <gtest/iutest_switch.hpp>
#include <utility>

using Pomdog::ConnectionList;
using Pomdog::Signal;

TEST(ConnectionList, Disconnect)
{
    Signal<void(std::string)> nameChanged;
    ConnectionList connections;
    std::string name;

    connections += nameChanged.Connect([&](std::string const& n) {
        name = n;
    });

    nameChanged("alice");
    EXPECT_EQ("alice", name);
    nameChanged("bob");
    EXPECT_EQ("bob", name);
    nameChanged("chuck");
    EXPECT_EQ("chuck", name);

    connections.Disconnect();

    nameChanged("norris");
    EXPECT_EQ("chuck", name);
}

TEST(ConnectionList, MoveAssignmentOperator)
{
    Signal<void(std::string)> nameChanged;
    std::string name;

    ConnectionList connections1;
    {
        ConnectionList connections2;

        connections2 += nameChanged.Connect([&](std::string const& n) {
            name = n;
        });

        nameChanged("alice");
        EXPECT_EQ("alice", name);

        connections1 = std::move(connections2);

        nameChanged("bob");
        EXPECT_EQ("bob", name);

        connections2.Disconnect();

        nameChanged("chuck");
        EXPECT_EQ("chuck", name);
    }

    nameChanged("norris");
    EXPECT_EQ("norris", name);

    connections1.Disconnect();

    nameChanged("gates");
    EXPECT_EQ("norris", name);
}

TEST(ConnectionList, ScopeGuard)
{
    Signal<void(int)> valueChanged;
    std::vector<int> integers;
    {
        ConnectionList connections;
        auto slot = [&](int n){ integers.push_back(n); };
        connections += valueChanged.Connect(slot);

        valueChanged(42);
        valueChanged(43);
    }
    valueChanged(44);

    ASSERT_EQ(2, integers.size());
    EXPECT_EQ(42, integers[0]);
    EXPECT_EQ(43, integers[1]);
}

TEST(ConnectionList, ScopeGuardWithThreeConnections)
{
    Signal<void(int)> valueChanged;
    std::vector<int> integers;
    {
        ConnectionList connections;
        auto slot = [&](int n){ integers.push_back(n); };
        connections += valueChanged.Connect(slot);
        connections += valueChanged.Connect(slot);
        connections += valueChanged.Connect(slot);

        valueChanged(42);
        valueChanged(43);
    }
    valueChanged(44);

    ASSERT_EQ(6, integers.size());
    EXPECT_EQ(42, integers[0]);
    EXPECT_EQ(42, integers[1]);
    EXPECT_EQ(42, integers[2]);
    EXPECT_EQ(43, integers[3]);
    EXPECT_EQ(43, integers[4]);
    EXPECT_EQ(43, integers[5]);
}

TEST(ConnectionList, QtStyleConnect)
{
    Signal<void(int)> valueChanged;
    std::vector<int> integers;
    {
        ConnectionList connect;
        auto slot = [&](int n){ integers.push_back(n); };
        connect(valueChanged, slot);
        connect(valueChanged, slot);
        connect(valueChanged, slot);

        valueChanged(42);
        valueChanged(43);
    }
    valueChanged(44);

    ASSERT_EQ(6, integers.size());
    EXPECT_EQ(42, integers[0]);
    EXPECT_EQ(42, integers[1]);
    EXPECT_EQ(42, integers[2]);
    EXPECT_EQ(43, integers[3]);
    EXPECT_EQ(43, integers[4]);
    EXPECT_EQ(43, integers[5]);
}

TEST(ConnectionList, QtStyleConnect_ReturnConnection)
{
    Signal<void(int)> valueChanged;
    std::vector<int> integers;
    Pomdog::Connection connection;
    {
        ConnectionList connect;
        auto slot = [&](int n){ integers.push_back(n); };
        connection = connect(valueChanged, slot);

        ASSERT_TRUE(connection);
        valueChanged(42);
        valueChanged(43);
        EXPECT_TRUE(connection);
    }
    EXPECT_FALSE(connection);
    valueChanged(44);

    ASSERT_EQ(2, integers.size());
    EXPECT_EQ(42, integers[0]);
    EXPECT_EQ(43, integers[1]);
}
