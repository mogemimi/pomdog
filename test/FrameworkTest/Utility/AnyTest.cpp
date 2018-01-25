// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Utility/Any.hpp>
#include <gtest/iutest_switch.hpp>
#include <memory>

using Pomdog::Any;

TEST(Any, FirstCase)
{
    Any x = std::string("Chuck Norris");

    ASSERT_EQ(typeid(std::string), x.Type());
    ASSERT_TRUE(x.Is<std::string>());
    EXPECT_EQ(std::string("Chuck Norris"), x.As<std::string>());
    EXPECT_EQ("Chuck Norris", x.As<std::string>());
}

TEST(Any, SharedPointer)
{
    {
        Any x = std::make_shared<std::string>("Chuck Norris");
        ASSERT_EQ(typeid(std::shared_ptr<std::string>), x.Type());
        ASSERT_TRUE(x.Is<std::shared_ptr<std::string>>());
        EXPECT_EQ(std::string("Chuck Norris"), *x.As<std::shared_ptr<std::string>>());
        EXPECT_EQ("Chuck Norris", *x.As<std::shared_ptr<std::string>>());
    }
    {
        auto original = std::make_shared<std::string>("Chuck Norris");
        auto copied = original;
        Any x = copied;

        ASSERT_EQ(typeid(std::shared_ptr<std::string>), x.Type());
        ASSERT_TRUE(x.Is<std::shared_ptr<std::string>>());
        EXPECT_EQ(std::string("Chuck Norris"), *x.As<std::shared_ptr<std::string>>());
        EXPECT_EQ("Chuck Norris", *x.As<std::shared_ptr<std::string>>());
        EXPECT_EQ(original, x.As<std::shared_ptr<std::string>>());
    }
    {
        auto original = std::make_shared<std::string>("Chuck Norris");
        auto copied = original;
        Any x = std::move(copied);

        ASSERT_EQ(typeid(std::shared_ptr<std::string>), x.Type());
        ASSERT_TRUE(x.Is<std::shared_ptr<std::string>>());
        EXPECT_EQ(std::string("Chuck Norris"), *x.As<std::shared_ptr<std::string>>());
        EXPECT_EQ("Chuck Norris", *x.As<std::shared_ptr<std::string>>());
        EXPECT_EQ(original, x.As<std::shared_ptr<std::string>>());
    }
    {
        Any x = std::make_shared<std::string>("Chuck Norris");
        ASSERT_EQ(typeid(std::shared_ptr<std::string>), x.Type());

        auto copied = x.As<std::shared_ptr<std::string>>();

        EXPECT_EQ("Chuck Norris", *copied);
        EXPECT_EQ(copied, x.As<std::shared_ptr<std::string>>());

        auto moved = std::move(x.As<std::shared_ptr<std::string>>());

        EXPECT_EQ(copied, moved);
        EXPECT_EQ("Chuck Norris", *moved);
        EXPECT_NE(moved, x.As<std::shared_ptr<std::string>>());
    }
}
