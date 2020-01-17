// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Utility/Any.hpp"
#include "catch.hpp"
#include <memory>

using Pomdog::Any;

TEST_CASE("Any", "[Any]")
{
    SECTION("First case")
    {
        Any x = std::string("Chuck Norris");

        REQUIRE(x.Type() == typeid(std::string));
        REQUIRE(x.Is<std::string>());
        REQUIRE(x.As<std::string>() == std::string{"Chuck Norris"});
        REQUIRE(x.As<std::string>() == "Chuck Norris");
    }
    SECTION("shared_ptr")
    {
        Any x = std::make_shared<std::string>("Chuck Norris");
        REQUIRE(x.Type() == typeid(std::shared_ptr<std::string>));
        REQUIRE(x.Is<std::shared_ptr<std::string>>());
        REQUIRE(*x.As<std::shared_ptr<std::string>>() == std::string{"Chuck Norris"});
        REQUIRE(*x.As<std::shared_ptr<std::string>>() == "Chuck Norris");
    }
    SECTION("copying with shared_ptr")
    {
        auto original = std::make_shared<std::string>("Chuck Norris");
        auto copied = original;
        Any x = copied;

        REQUIRE(x.Type() == typeid(std::shared_ptr<std::string>));
        REQUIRE(x.Is<std::shared_ptr<std::string>>());
        REQUIRE(*x.As<std::shared_ptr<std::string>>() == std::string{"Chuck Norris"});
        REQUIRE(*x.As<std::shared_ptr<std::string>>() == "Chuck Norris");
        REQUIRE(x.As<std::shared_ptr<std::string>>() == original);
    }
    SECTION("moving with shared_ptr")
    {
        auto original = std::make_shared<std::string>("Chuck Norris");
        auto copied = original;
        Any x = std::move(copied);

        REQUIRE(x.Type() == typeid(std::shared_ptr<std::string>));
        REQUIRE(x.Is<std::shared_ptr<std::string>>());
        REQUIRE(*x.As<std::shared_ptr<std::string>>() == std::string{"Chuck Norris"});
        REQUIRE(*x.As<std::shared_ptr<std::string>>() == "Chuck Norris");
        REQUIRE(x.As<std::shared_ptr<std::string>>() == original);
    }
    SECTION("copying and moving with shared_ptr")
    {
        Any x = std::make_shared<std::string>("Chuck Norris");
        REQUIRE(x.Type() == typeid(std::shared_ptr<std::string>));

        auto copied = x.As<std::shared_ptr<std::string>>();

        REQUIRE(*copied == "Chuck Norris");
        REQUIRE(x.As<std::shared_ptr<std::string>>() == copied);

        auto moved = std::move(x.As<std::shared_ptr<std::string>>());

        REQUIRE(copied == moved);
        REQUIRE(*moved == "Chuck Norris");
        REQUIRE(x.As<std::shared_ptr<std::string>>() != moved);
    }
}
