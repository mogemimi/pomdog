// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/array_view.h"
#include <catch_amalgamated.hpp>
#include <array>
#include <cstdint>
#include <string>

using namespace pomdog;

TEST_CASE("ArrayView", "[Network]")
{
    SECTION("basic usage")
    {
        std::string s = "hello";
        ArrayView<char const> view{s.data(), s.size()};

        REQUIRE(view.size() == s.size());
        REQUIRE(view.back() == s.back());
        REQUIRE(view.data() == s.data());
        REQUIRE(!view.empty());
    }
    SECTION("empty")
    {
        ArrayView<char const> view{nullptr, 0};

        REQUIRE(view.size() == 0);
        REQUIRE(view.data() == nullptr);
        REQUIRE(view.empty());
    }
    SECTION("view as other type array")
    {
        std::array<std::int64_t, 3> s = {0x1234567891234567, 0x1876543219876543, 0x1432587693214765};
        ArrayView<std::int64_t> view{s.data(), s.size()};

        REQUIRE(view.size() == s.size());
        REQUIRE(view.back() == s.back());
        REQUIRE(view.data() == s.data());
        REQUIRE(!view.empty());

        SECTION("to std::int64_t with const")
        {
            auto v = view.viewAs<std::int64_t const>();

            REQUIRE(v.size() == s.size());
            REQUIRE(v.back() == s.back());
            REQUIRE(v.data() == s.data());
            REQUIRE(!view.empty());
        }
        SECTION("to std::int32_t")
        {
            auto v = view.viewAs<std::int32_t>();

            REQUIRE(v.size() == 6);
            REQUIRE(v.back() == 0x14325876);
            REQUIRE(v.data() == reinterpret_cast<std::int32_t*>(s.data()));
            REQUIRE(!view.empty());
        }
        SECTION("to std::int16_t")
        {
            auto v = view.viewAs<std::int16_t>();

            REQUIRE(v.size() == 12);
            REQUIRE(v.back() == 0x1432);
            REQUIRE(v.data() == reinterpret_cast<std::int16_t*>(s.data()));
            REQUIRE(!view.empty());
        }
        SECTION("to std::int8_t")
        {
            auto v = view.viewAs<std::int8_t>();

            REQUIRE(v.size() == 24);
            REQUIRE(v.back() == 0x14);
            REQUIRE(v.data() == reinterpret_cast<std::int8_t*>(s.data()));
            REQUIRE(!view.empty());
        }
        SECTION("to std::int32_t with const")
        {
            auto v = view.viewAs<std::int32_t const>();

            REQUIRE(v.size() == 6);
            REQUIRE(v.back() == 0x14325876);
            REQUIRE(v.data() == reinterpret_cast<std::int32_t*>(s.data()));
            REQUIRE(!view.empty());
        }
        SECTION("to std::int16_t with const")
        {
            auto v = view.viewAs<std::int16_t const>();

            REQUIRE(v.size() == 12);
            REQUIRE(v.back() == 0x1432);
            REQUIRE(v.data() == reinterpret_cast<std::int16_t*>(s.data()));
            REQUIRE(!view.empty());
        }
        SECTION("to std::int8_t with const")
        {
            auto v = view.viewAs<std::int8_t const>();

            REQUIRE(v.size() == 24);
            REQUIRE(v.back() == 0x14);
            REQUIRE(v.data() == reinterpret_cast<std::int8_t*>(s.data()));
            REQUIRE(!view.empty());
        }
    }
}
