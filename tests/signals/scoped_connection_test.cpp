// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/signals/connection.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Connection;
using pomdog::ScopedConnection;
using pomdog::Signal;

TEST_CASE("ScopedConnection")
{
    SUBCASE("default construct")
    {
        ScopedConnection connection;
        REQUIRE_FALSE(connection.isConnected());
    }
    SUBCASE("construct from connection")
    {
        Signal<void()> signal;
        ScopedConnection connection{signal.connect([]() {})};

        REQUIRE(connection.isConnected());
    }
    SUBCASE("disconnect")
    {
        Signal<void()> signal;
        ScopedConnection connection{signal.connect([]() {})};

        REQUIRE(connection.isConnected());
        connection.disconnect();
        REQUIRE_FALSE(connection.isConnected());
    }
    SUBCASE("disconnect on destruction")
    {
        Signal<void()> signal;
        int counter = 0;

        {
            ScopedConnection connection{signal.connect([&counter]() { ++counter; })};

            signal();
            REQUIRE(counter == 1);
        }

        signal();
        REQUIRE(counter == 1);
    }
    SUBCASE("move construct")
    {
        Signal<void()> signal;
        ScopedConnection connection1{signal.connect([]() {})};

        REQUIRE(connection1.isConnected());

        ScopedConnection connection2{std::move(connection1)};
        REQUIRE(connection2.isConnected());
        REQUIRE_FALSE(connection1.isConnected());
    }
    SUBCASE("move assign")
    {
        Signal<void()> signal;
        ScopedConnection connection1{signal.connect([]() {})};

        REQUIRE(connection1.isConnected());

        ScopedConnection connection2;
        connection2 = std::move(connection1);
        REQUIRE(connection2.isConnected());
        REQUIRE_FALSE(connection1.isConnected());
    }
    SUBCASE("disconnect old on move assign")
    {
        Signal<void()> signal;
        int counter1 = 0;
        int counter2 = 0;

        ScopedConnection connection1{signal.connect([&counter1]() { ++counter1; })};
        ScopedConnection connection2{signal.connect([&counter2]() { ++counter2; })};

        signal();
        REQUIRE(counter1 == 1);
        REQUIRE(counter2 == 1);

        connection2 = std::move(connection1);

        signal();
        REQUIRE(counter1 == 2);
        // NOTE: Default move assignment of ScopedConnection does NOT disconnect
        // the old connection, it just moves the Connection member.
        // The slot connected to counter2 is still active.
        REQUIRE(counter2 == 2);
    }
    SUBCASE("ScopeGuard with values")
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
    SUBCASE("explicit disconnect")
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
    SUBCASE("detailed move assignment")
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
    SUBCASE("copy assignment event connection")
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
    SUBCASE("move assignment event connection")
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
