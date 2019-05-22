// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Signals/Event.hpp"
#include "catch.hpp"
#include <utility>
#include <cstdint>

using Pomdog::Event;

TEST_CASE("Event", "[Event]")
{
    SECTION("String")
    {
        Event event(std::string{"ChuckNorris"});

        REQUIRE(event.Is<std::string>());
        REQUIRE(event.As<std::string>() != nullptr);
        REQUIRE(*event.As<std::string>() == "ChuckNorris");
    }
    SECTION("Struct")
    {
        struct Message;

        {
            Event event(int{42});
            REQUIRE_FALSE(event.Is<Message>());
        }

        struct Message
        {
            int code;
            std::string description;
        };

        Event event(Message{42, "the answer to life the universe and everything"});

        REQUIRE(event.Is<Message>());
        REQUIRE_FALSE(event.Is<int>());
        REQUIRE_FALSE(event.Is<std::string>());

        REQUIRE(event.As<Message>() != nullptr);
        REQUIRE(event.As<Message>()->code == 42);
        REQUIRE(event.As<Message>()->description == "the answer to life the universe and everything");
    }
    SECTION("Int")
    {
        Event event(int{42});

        REQUIRE(event.Is<int>());
        REQUIRE_FALSE(event.Is<char>());
        REQUIRE_FALSE(event.Is<float>());
        REQUIRE_FALSE(event.Is<double>());
        REQUIRE_FALSE(event.Is<unsigned int>());
        REQUIRE_FALSE(event.Is<std::string>());

        REQUIRE(event.As<int>() != nullptr);
        REQUIRE(*event.As<int>() == 42);
    }
    SECTION("Int32")
    {
        Event event(std::int32_t{42});

        REQUIRE(event.Is<std::int32_t>());
        REQUIRE_FALSE(event.Is<std::int8_t>());
        REQUIRE_FALSE(event.Is<std::int16_t>());
        REQUIRE_FALSE(event.Is<std::int64_t>());
        REQUIRE_FALSE(event.Is<std::uint32_t>());

        REQUIRE(event.As<std::int32_t>() != nullptr);
        REQUIRE(*event.As<std::int32_t>() == 42);
    }
    SECTION("Int64")
    {
        Event event(std::int64_t{42});

        REQUIRE(event.Is<std::int64_t>());
        REQUIRE_FALSE(event.Is<std::int8_t>());
        REQUIRE_FALSE(event.Is<std::int16_t>());
        REQUIRE_FALSE(event.Is<std::int32_t>());
        REQUIRE_FALSE(event.Is<std::uint64_t>());

        REQUIRE(event.As<std::int64_t>() != nullptr);
        REQUIRE(*event.As<std::int64_t>() == 42);
    }
}
