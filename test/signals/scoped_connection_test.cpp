// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/signals/connection.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/signals/signal.h"
#include <catch_amalgamated.hpp>
#include <utility>

using pomdog::Connection;
using pomdog::ScopedConnection;
using pomdog::Signal;

TEST_CASE("ScopeGuard", "[ScopedConnection]")
{
    SECTION("ScopeGuard")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;
        {
            ScopedConnection connection;
            auto slot = [&](int n) { integers.push_back(n); };
            connection = valueChanged.connect(slot);
            REQUIRE(connection.isConnected());

            valueChanged(42);
            REQUIRE(connection.isConnected());

            valueChanged(43);
            REQUIRE(connection.isConnected());
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
        connection = valueChanged.connect(slot);
        REQUIRE(connection.isConnected());

        valueChanged(42);
        REQUIRE(connection.isConnected());

        valueChanged(43);
        REQUIRE(connection.isConnected());

        connection.disconnect();
        valueChanged(44);
        REQUIRE_FALSE(connection.isConnected());

        REQUIRE(integers.size() == 2);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
    }
    SECTION("move assignment")
    {
        Signal<void(int)> valueChanged;
        std::vector<int> integers;

        ScopedConnection connection1;
        REQUIRE_FALSE(connection1.isConnected());

        {
            auto slot = [&](int n) { integers.push_back(n); };
            ScopedConnection connection2 = valueChanged.connect(slot);
            REQUIRE_FALSE(connection1.isConnected());
            REQUIRE(connection2.isConnected());

            valueChanged(42);
            REQUIRE_FALSE(connection1.isConnected());
            REQUIRE(connection2.isConnected());

            connection1 = std::move(connection2);
            REQUIRE(connection1.isConnected());
            REQUIRE_FALSE(connection2.isConnected());

            valueChanged(43);
        }

        REQUIRE(connection1.isConnected());

        valueChanged(44);
        REQUIRE(connection1.isConnected());

        connection1.disconnect();
        REQUIRE_FALSE(connection1.isConnected());

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
            Connection connection2 = valueChanged.connect(slot);
            valueChanged(42);
            connection1 = connection2;
            valueChanged(43);
        }

        valueChanged(44);
        connection1.disconnect();
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
            Connection connection2 = valueChanged.connect(slot);

            valueChanged(42);
            connection1 = std::move(connection2);
            valueChanged(43);
            connection2.disconnect();
            valueChanged(44);
        }

        valueChanged(45);
        connection1.disconnect();
        valueChanged(46);

        REQUIRE(integers.size() == 4);
        REQUIRE(integers[0] == 42);
        REQUIRE(integers[1] == 43);
        REQUIRE(integers[2] == 44);
        REQUIRE(integers[3] == 45);
    }
}
