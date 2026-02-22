// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/signals/signal.h"
#include "pomdog/signals/signal_helpers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Signal;
using pomdog::signals::connectSingleShot;

TEST_CASE("SignalHelpers")
{
    SUBCASE("connectSingleShot")
    {
        Signal<void(int)> signal;
        int counter = 0;

        connectSingleShot(signal, [&counter](int x) {
            counter += x;
        });

        signal(1);
        REQUIRE(counter == 1);

        signal(10);
        REQUIRE(counter == 1);

        signal(100);
        REQUIRE(counter == 1);
    }
    SUBCASE("connectSingleShot multiple slots")
    {
        Signal<void(int)> signal;
        int counter1 = 0;
        int counter2 = 0;

        connectSingleShot(signal, [&counter1](int x) {
            counter1 += x;
        });
        connectSingleShot(signal, [&counter2](int x) {
            counter2 += x * 2;
        });

        signal(1);
        REQUIRE(counter1 == 1);
        REQUIRE(counter2 == 2);

        signal(10);
        REQUIRE(counter1 == 1);
        REQUIRE(counter2 == 2);
    }
    SUBCASE("connectSingleShot Disconnect")
    {
        Signal<void(std::string const&)> nameChanged;

        std::vector<std::string> result;
        auto connection = connectSingleShot(nameChanged, [&](std::string const& n) {
            result.push_back(n);
        });

        REQUIRE(result.empty());
        REQUIRE(connection.isConnected());

        connection.disconnect();
        REQUIRE_FALSE(connection.isConnected());

        nameChanged("chuck");
        REQUIRE(result.empty());
        REQUIRE_FALSE(connection.isConnected());

        nameChanged("norris");
        REQUIRE(result.empty());
        REQUIRE_FALSE(connection.isConnected());
    }
}
