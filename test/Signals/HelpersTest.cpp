// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Signals/Signal.hpp>
#include <Pomdog/Signals/Helpers.hpp>
#include <gtest/iutest_switch.hpp>
#include <string>
#include <utility>
#include <vector>

using Pomdog::Signal;
using namespace Pomdog::Signals;

TEST(Signals_Helpers, ConnectSingleShot)
{
    Pomdog::Signal<void(std::string const&)> nameChanged;

    std::vector<std::string> result;
    auto connection = ConnectSingleShot(nameChanged, [&](std::string const& n) {
        result.push_back(n);
    });

    ASSERT_TRUE(result.empty());
    EXPECT_TRUE(connection);

    nameChanged("chuck");
    EXPECT_EQ(1, result.size());
    EXPECT_EQ("chuck", result.back());
    EXPECT_FALSE(connection);

    nameChanged("norris");
    EXPECT_EQ(1, result.size());
    EXPECT_FALSE(connection);
}

TEST(Signals_Helpers, ConnectSingleShot_Disconnect)
{
    Pomdog::Signal<void(std::string const&)> nameChanged;

    std::vector<std::string> result;
    auto connection = ConnectSingleShot(nameChanged, [&](std::string const& n) {
        result.push_back(n);
    });

    ASSERT_TRUE(result.empty());
    EXPECT_TRUE(connection);

    connection.Disconnect();
    EXPECT_FALSE(connection);

    nameChanged("chuck");
    EXPECT_TRUE(result.empty());
    EXPECT_FALSE(connection);

    nameChanged("norris");
    EXPECT_TRUE(result.empty());
    EXPECT_FALSE(connection);
}
