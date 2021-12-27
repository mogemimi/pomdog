// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/signals/signal.h"
#include "pomdog/signals/signal_helpers.h"
#include <catch_amalgamated.hpp>
#include <string>
#include <utility>
#include <vector>

using pomdog::Signal;
using namespace pomdog::Signals;

TEST_CASE("helpers for Signals", "[Signals]")
{
    SECTION("ConnectSingleShot")
    {
        pomdog::Signal<void(std::string const&)> nameChanged;

        std::vector<std::string> result;
        auto connection = ConnectSingleShot(nameChanged, [&](std::string const& n) {
            result.push_back(n);
        });

        REQUIRE(result.empty());
        REQUIRE(connection.IsConnected());

        nameChanged("chuck");
        REQUIRE(result.size() == 1);
        REQUIRE(result.back() == "chuck");
        REQUIRE_FALSE(connection.IsConnected());

        nameChanged("norris");
        REQUIRE(result.size() == 1);
        REQUIRE_FALSE(connection.IsConnected());
    }
    SECTION("ConnectSingleShot Disconnect")
    {
        pomdog::Signal<void(std::string const&)> nameChanged;

        std::vector<std::string> result;
        auto connection = ConnectSingleShot(nameChanged, [&](std::string const& n) {
            result.push_back(n);
        });

        REQUIRE(result.empty());
        REQUIRE(connection.IsConnected());

        connection.Disconnect();
        REQUIRE_FALSE(connection.IsConnected());

        nameChanged("chuck");
        REQUIRE(result.empty());
        REQUIRE_FALSE(connection.IsConnected());

        nameChanged("norris");
        REQUIRE(result.empty());
        REQUIRE_FALSE(connection.IsConnected());
    }
}
