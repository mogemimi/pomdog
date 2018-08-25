// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Input/Keys.hpp>
#include <gtest/iutest_switch.hpp>
#include <type_traits>

using Pomdog::Keys;

TEST(Keys, None)
{
    static_assert(std::is_same<std::uint8_t, typename std::underlying_type<Keys>::type>::value,
        "Keys is std::uint8_t.");
    EXPECT_EQ(0, static_cast<std::uint8_t>(Keys::None));

    EXPECT_TRUE(Keys::None != Keys::A);
    EXPECT_TRUE(Keys::None != Keys::B);
    EXPECT_TRUE(Keys::None != Keys::C);
    EXPECT_TRUE(Keys::None != Keys::D);
    EXPECT_TRUE(Keys::None != Keys::E);
    EXPECT_TRUE(Keys::None != Keys::F);
    EXPECT_TRUE(Keys::None != Keys::G);
    EXPECT_TRUE(Keys::None != Keys::H);
    EXPECT_TRUE(Keys::None != Keys::I);
    EXPECT_TRUE(Keys::None != Keys::J);
    EXPECT_TRUE(Keys::None != Keys::K);
    EXPECT_TRUE(Keys::None != Keys::L);
    EXPECT_TRUE(Keys::None != Keys::M);
    EXPECT_TRUE(Keys::None != Keys::N);
    EXPECT_TRUE(Keys::None != Keys::O);
    EXPECT_TRUE(Keys::None != Keys::P);
    EXPECT_TRUE(Keys::None != Keys::Q);
    EXPECT_TRUE(Keys::None != Keys::R);
    EXPECT_TRUE(Keys::None != Keys::S);
    EXPECT_TRUE(Keys::None != Keys::T);
    EXPECT_TRUE(Keys::None != Keys::U);
    EXPECT_TRUE(Keys::None != Keys::V);
    EXPECT_TRUE(Keys::None != Keys::W);
    EXPECT_TRUE(Keys::None != Keys::X);
    EXPECT_TRUE(Keys::None != Keys::Y);
    EXPECT_TRUE(Keys::None != Keys::Z);
}
