// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/signals/connection.h"
#include "pomdog/signals/connection_list.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Connection;
using pomdog::ConnectionList;
using pomdog::Signal;

TEST_CASE("ConnectionList")
{
    SUBCASE("default constructor")
    {
        ConnectionList connections;
        // Should not crash
    }
    SUBCASE("add connection")
    {
        Signal<void()> signal;
        ConnectionList connections;
        int counter = 0;

        connections += signal.connect([&counter]() { ++counter; });

        signal();
        REQUIRE(counter == 1);
    }
    SUBCASE("add multiple connections")
    {
        Signal<void()> signal;
        ConnectionList connections;
        int counter = 0;

        connections += signal.connect([&counter]() { ++counter; });
        connections += signal.connect([&counter]() { counter += 10; });

        signal();
        REQUIRE(counter == 11);
    }
    SUBCASE("disconnect")
    {
        Signal<void()> signal;
        ConnectionList connections;
        int counter = 0;

        connections += signal.connect([&counter]() { ++counter; });
        connections += signal.connect([&counter]() { counter += 10; });

        signal();
        REQUIRE(counter == 11);

        connections.disconnect();

        signal();
        REQUIRE(counter == 11);
    }
    SUBCASE("disconnect on destruction")
    {
        Signal<void()> signal;
        int counter = 0;

        {
            ConnectionList connections;
            connections += signal.connect([&counter]() { ++counter; });

            signal();
            REQUIRE(counter == 1);
        }

        signal();
        REQUIRE(counter == 1);
    }
    SUBCASE("move construct")
    {
        Signal<void()> signal;
        ConnectionList connections1;
        int counter = 0;

        connections1 += signal.connect([&counter]() { ++counter; });

        signal();
        REQUIRE(counter == 1);

        ConnectionList connections2{std::move(connections1)};

        signal();
        REQUIRE(counter == 2);

        connections2.disconnect();

        signal();
        REQUIRE(counter == 2);
    }
    SUBCASE("move assign")
    {
        Signal<void()> signal;
        ConnectionList connections1;
        ConnectionList connections2;
        int counter = 0;

        connections1 += signal.connect([&counter]() { ++counter; });

        signal();
        REQUIRE(counter == 1);

        connections2 = std::move(connections1);

        signal();
        REQUIRE(counter == 2);

        connections2.disconnect();

        signal();
        REQUIRE(counter == 2);
    }
    SUBCASE("move assignment operator")
    {
        Signal<void(std::string)> nameChanged;
        std::string name;

        ConnectionList connections1;
        {
            ConnectionList connections2;

            connections2 += nameChanged.connect([&](std::string const& n) {
                name = n;
            });

            nameChanged("alice");
            REQUIRE(name == "alice");

            connections1 = std::move(connections2);

            nameChanged("bob");
            REQUIRE(name == "bob");

            connections2.disconnect();

            nameChanged("chuck");
            REQUIRE(name == "chuck");
        }

        nameChanged("norris");
        REQUIRE(name == "norris");

        connections1.disconnect();

        nameChanged("gates");
        REQUIRE(name == "norris");
    }
    SUBCASE("scope guard")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;
        {
            ConnectionList connections;
            auto slot = [&](int n) { integers.push_back(n); };
            connections += valueChanged.connect(slot);

            valueChanged(42);
            valueChanged(43);
        }
        valueChanged(44);

        REQUIRE(integers.size() == 2);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
    }
    SUBCASE("scope guard with three connections")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;
        {
            ConnectionList connections;
            auto slot = [&](int n) { integers.push_back(n); };
            connections += valueChanged.connect(slot);
            connections += valueChanged.connect(slot);
            connections += valueChanged.connect(slot);

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
    SUBCASE("Qt style connect")
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
    SUBCASE("Qt style connect returning Connection")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;
        Connection connection;
        {
            ConnectionList connect;
            auto slot = [&](int n) { integers.push_back(n); };
            connection = connect(valueChanged, slot);

            REQUIRE(connection.isConnected());
            valueChanged(42);
            valueChanged(43);
            REQUIRE(connection.isConnected());
        }
        REQUIRE_FALSE(connection.isConnected());
        valueChanged(44);

        REQUIRE(integers.size() == 2);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
    }
}
