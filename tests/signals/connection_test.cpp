// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/signals/connection.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Connection;
using pomdog::Signal;

TEST_CASE("Connection")
{
    SUBCASE("default construct")
    {
        Connection connection;
        REQUIRE_FALSE(connection.isConnected());
    }
    SUBCASE("isConnected")
    {
        Signal<void()> signal;
        auto connection = signal.connect([]() {});

        REQUIRE(connection.isConnected());
    }
    SUBCASE("disconnect")
    {
        Signal<void()> signal;
        auto connection = signal.connect([]() {});

        REQUIRE(connection.isConnected());
        connection.disconnect();
        REQUIRE_FALSE(connection.isConnected());
    }
    SUBCASE("move construct")
    {
        Signal<void()> signal;
        auto connection1 = signal.connect([]() {});

        REQUIRE(connection1.isConnected());

        Connection connection2{std::move(connection1)};
        REQUIRE(connection2.isConnected());
    }
    SUBCASE("move assign")
    {
        Signal<void()> signal;
        auto connection1 = signal.connect([]() {});

        REQUIRE(connection1.isConnected());

        Connection connection2;
        connection2 = std::move(connection1);
        REQUIRE(connection2.isConnected());
    }
    SUBCASE("disconnect old on move assign")
    {
        Signal<void()> signal;
        int counter1 = 0;
        int counter2 = 0;

        auto connection1 = signal.connect([&counter1]() { ++counter1; });
        auto connection2 = signal.connect([&counter2]() { ++counter2; });

        signal();
        REQUIRE(counter1 == 1);
        REQUIRE(counter2 == 1);

        connection2 = std::move(connection1);

        signal();
        REQUIRE(counter1 == 2);
        // NOTE: Move assignment does NOT disconnect the old connection,
        // it only moves the unique_ptr (old body is deleted, not disconnected)
        // The slot connected to counter2 is still active
        REQUIRE(counter2 == 2);
    }
    SUBCASE("disconnect with Signal")
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
    SUBCASE("copy assignment operator")
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
    SUBCASE("move assignment operator")
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
