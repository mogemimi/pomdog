// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/signals/connection.h"
#include "pomdog/signals/signal.h"
#include <catch_amalgamated.hpp>
#include <utility>

using pomdog::Connection;
using pomdog::Signal;

TEST_CASE("Connection", "[Connection]")
{
    SECTION("Disconnect")
    {
        Signal<void(std::string)> nameChanged;
        Connection connection;
        std::string name;

        connection = nameChanged.connect([&](std::string const& n) {
            name = n;
        });

        nameChanged("alice");
        REQUIRE(name == "alice");
        nameChanged("bob");
        REQUIRE(name == "bob");
        nameChanged("chuck");
        REQUIRE(name == "chuck");

        connection.disconnect();

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

            connection2 = nameChanged.connect([&](std::string const& n) {
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

        connection1.disconnect();

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

            connection2 = nameChanged.connect([&](std::string const& n) {
                name = n;
            });

            nameChanged("alice");
            REQUIRE(name == "alice");

            connection1 = std::move(connection2);

            nameChanged("bob");
            REQUIRE(name == "bob");

            connection2.disconnect();

            nameChanged("chuck");
            REQUIRE(name == "chuck");
        }

        nameChanged("norris");
        REQUIRE(name == "norris");

        connection1.disconnect();

        nameChanged("gates");
        REQUIRE(name == "norris");
    }
}
