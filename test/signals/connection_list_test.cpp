// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/signals/connection_list.hpp"
#include "pomdog/signals/signal.hpp"
#include <catch_amalgamated.hpp>
#include <utility>

using pomdog::ConnectionList;
using pomdog::Signal;

TEST_CASE("Disconnect", "[ConnectionList]")
{
    SECTION("Disconnect")
    {
        Signal<void(std::string)> nameChanged;
        ConnectionList connections;
        std::string name;

        connections += nameChanged.Connect([&](std::string const& n) {
            name = n;
        });

        nameChanged("alice");
        REQUIRE(name == "alice");
        nameChanged("bob");
        REQUIRE(name == "bob");
        nameChanged("chuck");
        REQUIRE(name == "chuck");

        connections.Disconnect();

        nameChanged("norris");
        REQUIRE(name == "chuck");
    }
    SECTION("MoveAssignmentOperator")
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
            REQUIRE(name == "alice");

            connections1 = std::move(connections2);

            nameChanged("bob");
            REQUIRE(name == "bob");

            connections2.Disconnect();

            nameChanged("chuck");
            REQUIRE(name == "chuck");
        }

        nameChanged("norris");
        REQUIRE(name == "norris");

        connections1.Disconnect();

        nameChanged("gates");
        REQUIRE(name == "norris");
    }
    SECTION("ScopeGuard")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;
        {
            ConnectionList connections;
            auto slot = [&](int n) { integers.push_back(n); };
            connections += valueChanged.Connect(slot);

            valueChanged(42);
            valueChanged(43);
        }
        valueChanged(44);

        REQUIRE(integers.size() == 2);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
    }
    SECTION("ScopeGuardWithThreeConnections")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;
        {
            ConnectionList connections;
            auto slot = [&](int n) { integers.push_back(n); };
            connections += valueChanged.Connect(slot);
            connections += valueChanged.Connect(slot);
            connections += valueChanged.Connect(slot);

            valueChanged(42);
            valueChanged(43);
        }
        valueChanged(44);

        REQUIRE(integers.size() == 6);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 42);
        REQUIRE(integers[2] == 42);
        REQUIRE(integers[3] == 43);
        REQUIRE(integers[4] == 43);
        REQUIRE(integers[5] == 43);
    }
    SECTION("QtStyleConnect")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;
        {
            ConnectionList connect;
            auto slot = [&](int n) { integers.push_back(n); };
            connect(valueChanged, slot);
            connect(valueChanged, slot);
            connect(valueChanged, slot);

            valueChanged(42);
            valueChanged(43);
        }
        valueChanged(44);

        REQUIRE(integers.size() == 6);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 42);
        REQUIRE(integers[2] == 42);
        REQUIRE(integers[3] == 43);
        REQUIRE(integers[4] == 43);
        REQUIRE(integers[5] == 43);
    }
    SECTION("QtStyleConnect_ReturnConnection")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;
        pomdog::Connection connection;
        {
            ConnectionList connect;
            auto slot = [&](int n) { integers.push_back(n); };
            connection = connect(valueChanged, slot);

            REQUIRE(connection.IsConnected());
            valueChanged(42);
            valueChanged(43);
            REQUIRE(connection.IsConnected());
        }
        REQUIRE_FALSE(connection.IsConnected());
        valueChanged(44);

        REQUIRE(integers.size() == 2);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
    }
}
