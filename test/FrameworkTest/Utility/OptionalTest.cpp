// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include <Pomdog/Utility/Optional.hpp>
#include <gtest/iutest_switch.hpp>
#include <memory>

using Pomdog::Optional;

TEST(Optional, FirstCase)
{
    Optional<int> number;
    ASSERT_FALSE(number);
    ASSERT_TRUE(!number);

    number = 42;
    ASSERT_TRUE(number);
    ASSERT_FALSE(!number);
    ASSERT_EQ(42, *number);
    ASSERT_EQ(42, number.value());

    number = Pomdog::NullOpt;
    ASSERT_FALSE(number);
    ASSERT_TRUE(!number);
}

TEST(Optional, CopyConstructor)
{
    Optional<int> number;
    ASSERT_FALSE(number);

    {
        auto other = number;
        ASSERT_FALSE(other);
        ASSERT_TRUE(!other);
    }

    number = 42;
    ASSERT_TRUE(number);
    ASSERT_EQ(42, *number);

    {
        auto other = number;
        ASSERT_TRUE(other);
        ASSERT_FALSE(!other);
        ASSERT_EQ(42, *other);
        ASSERT_EQ(42, other.value());
    }

    number = Pomdog::NullOpt;
    ASSERT_FALSE(number);

    {
        auto other = number;
        ASSERT_FALSE(other);
        ASSERT_TRUE(!other);
    }
}
