// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Signals/Connection.hpp>
#include <Pomdog/Signals/Signal.hpp>
#include <gtest/iutest_switch.hpp>
#include <utility>

using Pomdog::Connection;
using Pomdog::Signal;

TEST(Connection, Disconnect)
{
    Signal<void(std::string)> nameChanged;
    Connection connection;
    std::string name;

    connection = nameChanged.Connect([&](std::string const& n) {
        name = n;
    });

    nameChanged("alice");
    EXPECT_EQ("alice", name);
    nameChanged("bob");
    EXPECT_EQ("bob", name);
    nameChanged("chuck");
    EXPECT_EQ("chuck", name);

    connection.Disconnect();

    nameChanged("norris");
    EXPECT_EQ("chuck", name);
}

TEST(Connection, CopyAssignmentOperator)
{
    Signal<void(std::string)> nameChanged;
    std::string name;

    Connection connection1;
    {
        Connection connection2;

        connection2 = nameChanged.Connect([&](std::string const& n) {
            name = n;
        });

        nameChanged("alice");
        EXPECT_EQ("alice", name);

        connection1 = connection2;

        nameChanged("bob");
        EXPECT_EQ("bob", name);
    }

    nameChanged("chuck");
    EXPECT_EQ("chuck", name);

    connection1.Disconnect();

    nameChanged("norris");
    EXPECT_EQ("chuck", name);
}

TEST(Connection, MoveAssignmentOperator)
{
    Signal<void(std::string)> nameChanged;
    std::string name;

    Connection connection1;
    {
        Connection connection2;

        connection2 = nameChanged.Connect([&](std::string const& n) {
            name = n;
        });

        nameChanged("alice");
        EXPECT_EQ("alice", name);

        connection1 = std::move(connection2);

        nameChanged("bob");
        EXPECT_EQ("bob", name);

        connection2.Disconnect();

        nameChanged("chuck");
        EXPECT_EQ("chuck", name);
    }

    nameChanged("norris");
    EXPECT_EQ("norris", name);

    connection1.Disconnect();

    nameChanged("gates");
    EXPECT_EQ("norris", name);
}
