// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Signals/Event.hpp>
#include <gtest/iutest_switch.hpp>
#include <utility>
#include <cstdint>

using Pomdog::Event;

TEST(Event, String)
{
    Event event(std::string{"ChuckNorris"});

    EXPECT_TRUE(event.Is<std::string>());
    ASSERT_NE(nullptr, event.As<std::string>());
    EXPECT_EQ("ChuckNorris", *event.As<std::string>());
}

TEST(Event, Struct)
{
    struct Message;

    {
        Event event(int{42});
        EXPECT_FALSE(event.Is<Message>());
    }

    struct Message
    {
        int code;
        std::string description;
    };

    Event event(Message{42, "the answer to life the universe and everything"});

    EXPECT_TRUE(event.Is<Message>());
    ASSERT_FALSE(event.Is<int>());
    ASSERT_FALSE(event.Is<std::string>());

    ASSERT_NE(nullptr, event.As<Message>());
    EXPECT_EQ(42, event.As<Message>()->code);
    EXPECT_EQ("the answer to life the universe and everything", event.As<Message>()->description);
}

TEST(Event, Int)
{
    Event event(int{42});

    EXPECT_TRUE(event.Is<int>());
    ASSERT_FALSE(event.Is<char>());
    ASSERT_FALSE(event.Is<float>());
    ASSERT_FALSE(event.Is<double>());
    ASSERT_FALSE(event.Is<unsigned int>());
    ASSERT_FALSE(event.Is<std::string>());

    ASSERT_NE(nullptr, event.As<int>());
    EXPECT_EQ(42, *event.As<int>());
}

TEST(Event, Int32)
{
    Event event(std::int32_t{42});

    EXPECT_TRUE(event.Is<std::int32_t>());
    ASSERT_FALSE(event.Is<std::int8_t>());
    ASSERT_FALSE(event.Is<std::int16_t>());
    ASSERT_FALSE(event.Is<std::int64_t>());
    ASSERT_FALSE(event.Is<std::uint32_t>());

    ASSERT_NE(nullptr, event.As<std::int32_t>());
    EXPECT_EQ(42, *event.As<std::int32_t>());
}

TEST(Event, Int64)
{
    Event event(std::int64_t{42});

    EXPECT_TRUE(event.Is<std::int64_t>());
    ASSERT_FALSE(event.Is<std::int8_t>());
    ASSERT_FALSE(event.Is<std::int16_t>());
    ASSERT_FALSE(event.Is<std::int32_t>());
    ASSERT_FALSE(event.Is<std::uint64_t>());

    ASSERT_NE(nullptr, event.As<std::int64_t>());
    EXPECT_EQ(42, *event.As<std::int64_t>());
}
