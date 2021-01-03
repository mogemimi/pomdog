// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Signals/Connection.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include "catch.hpp"
#include <utility>

using Pomdog::Connection;
using Pomdog::ScopedConnection;
using Pomdog::Signal;

TEST_CASE("ScopeGuard", "[ScopedConnection]")
{
    SECTION("ScopeGuard")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;
        {
            ScopedConnection connection;
            auto slot = [&](int n) { integers.push_back(n); };
            connection = valueChanged.Connect(slot);
            REQUIRE(connection.IsConnected());

            valueChanged(42);
            REQUIRE(connection.IsConnected());

            valueChanged(43);
            REQUIRE(connection.IsConnected());
        }
        valueChanged(44);

        REQUIRE(integers.size() == 2);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
    }
    SECTION("explicit disconnect")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;
        ScopedConnection connection;

        auto slot = [&](int n) { integers.push_back(n); };
        connection = valueChanged.Connect(slot);
        REQUIRE(connection.IsConnected());

        valueChanged(42);
        REQUIRE(connection.IsConnected());

        valueChanged(43);
        REQUIRE(connection.IsConnected());

        connection.Disconnect();
        valueChanged(44);
        REQUIRE_FALSE(connection.IsConnected());

        REQUIRE(integers.size() == 2);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
    }
    SECTION("move assignment")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        ScopedConnection connection1;
        REQUIRE_FALSE(connection1.IsConnected());

        {
            auto slot = [&](int n) { integers.push_back(n); };
            ScopedConnection connection2 = valueChanged.Connect(slot);
            REQUIRE_FALSE(connection1.IsConnected());
            REQUIRE(connection2.IsConnected());

            valueChanged(42);
            REQUIRE_FALSE(connection1.IsConnected());
            REQUIRE(connection2.IsConnected());

            connection1 = std::move(connection2);
            REQUIRE(connection1.IsConnected());
            REQUIRE_FALSE(connection2.IsConnected());

            valueChanged(43);
        }

        REQUIRE(connection1.IsConnected());

        valueChanged(44);
        REQUIRE(connection1.IsConnected());

        connection1.Disconnect();
        REQUIRE_FALSE(connection1.IsConnected());

        valueChanged(45);

        REQUIRE(integers.size() == 3);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
        REQUIRE(integers[2] == 44);
    }
    SECTION("copy assignment event connection")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        ScopedConnection connection1;
        {
            auto slot = [&](int n) { integers.push_back(n); };
            Connection connection2 = valueChanged.Connect(slot);
            valueChanged(42);
            connection1 = connection2;
            valueChanged(43);
        }

        valueChanged(44);
        connection1.Disconnect();
        valueChanged(45);

        REQUIRE(integers.size() == 3);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
        REQUIRE(integers[2] == 44);
    }
    SECTION("move assignment event connection")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        ScopedConnection connection1;
        {
            auto slot = [&](int n) { integers.push_back(n); };
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

        REQUIRE(integers.size() == 4);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
        REQUIRE(integers[2] == 44);
        REQUIRE(integers[3] == 45);
    }
}
