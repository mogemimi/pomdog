// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Signals/Helpers.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include "catch.hpp"
#include <string>
#include <utility>
#include <vector>

using Pomdog::Signal;
using namespace Pomdog::Signals;

TEST_CASE("helpers for Signals", "[Signals]")
{
    SECTION("ConnectSingleShot")
    {
        Pomdog::Signal<void(std::string const&)> nameChanged;

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
        Pomdog::Signal<void(std::string const&)> nameChanged;

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
