// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Signals/Connection.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include "catch.hpp"
#include <utility>

using Pomdog::Connection;
using Pomdog::Signal;

TEST_CASE("Connection", "[Connection]")
{
    SECTION("Disconnect")
    {
        Signal<void(std::string)> nameChanged;
        Connection connection;
        std::string name;

        connection = nameChanged.Connect([&](std::string const& n) {
            name = n;
        });

        nameChanged("alice");
        REQUIRE(name == "alice");
        nameChanged("bob");
        REQUIRE(name == "bob");
        nameChanged("chuck");
        REQUIRE(name == "chuck");

        connection.Disconnect();

        nameChanged("norris");
        REQUIRE(name == "chuck");
    }
    SECTION("CopyAssignmentOperator")
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
            REQUIRE(name == "alice");

            connection1 = connection2;

            nameChanged("bob");
            REQUIRE(name == "bob");
        }

        nameChanged("chuck");
        REQUIRE(name == "chuck");

        connection1.Disconnect();

        nameChanged("norris");
        REQUIRE(name == "chuck");
    }
    SECTION("MoveAssignmentOperator")
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
            REQUIRE(name == "alice");

            connection1 = std::move(connection2);

            nameChanged("bob");
            REQUIRE(name == "bob");

            connection2.Disconnect();

            nameChanged("chuck");
            REQUIRE(name == "chuck");
        }

        nameChanged("norris");
        REQUIRE(name == "norris");

        connection1.Disconnect();

        nameChanged("gates");
        REQUIRE(name == "norris");
    }
}
