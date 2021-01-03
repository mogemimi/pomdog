// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Network/ArrayView.hpp"
#include "catch.hpp"
#include <array>
#include <cstdint>
#include <string>

using namespace Pomdog;

TEST_CASE("ArrayView", "[Network]")
{
    SECTION("basic usage") {
        std::string s = "hello";
        ArrayView<char const> view{s.data(), s.size()};

        REQUIRE(view.GetSize() == s.size());
        REQUIRE(view.GetBack() == s.back());
        REQUIRE(view.GetData() == s.data());
        REQUIRE(!view.IsEmpty());
    }
    SECTION("empty") {
        ArrayView<char const> view{nullptr, 0};

        REQUIRE(view.GetSize() == 0);
        REQUIRE(view.GetData() == nullptr);
        REQUIRE(view.IsEmpty());
    }
    SECTION("view as other type array") {
        std::array<std::int64_t, 3> s = {0x1234567891234567, 0x1876543219876543, 0x1432587693214765};
        ArrayView<std::int64_t> view{s.data(), s.size()};

        REQUIRE(view.GetSize() == s.size());
        REQUIRE(view.GetBack() == s.back());
        REQUIRE(view.GetData() == s.data());
        REQUIRE(!view.IsEmpty());

        SECTION("to std::int64_t with const") {
            auto v = view.ViewAs<std::int64_t const>();

            REQUIRE(v.GetSize() == s.size());
            REQUIRE(v.GetBack() == s.back());
            REQUIRE(v.GetData() == s.data());
            REQUIRE(!view.IsEmpty());
        }
        SECTION("to std::int32_t") {
            auto v = view.ViewAs<std::int32_t>();

            REQUIRE(v.GetSize() == 6);
            REQUIRE(v.GetBack() == 0x14325876);
            REQUIRE(v.GetData() == reinterpret_cast<std::int32_t*>(s.data()));
            REQUIRE(!view.IsEmpty());
        }
        SECTION("to std::int16_t") {
            auto v = view.ViewAs<std::int16_t>();

            REQUIRE(v.GetSize() == 12);
            REQUIRE(v.GetBack() == 0x1432);
            REQUIRE(v.GetData() == reinterpret_cast<std::int16_t*>(s.data()));
            REQUIRE(!view.IsEmpty());
        }
        SECTION("to std::int8_t") {
            auto v = view.ViewAs<std::int8_t>();

            REQUIRE(v.GetSize() == 24);
            REQUIRE(v.GetBack() == 0x14);
            REQUIRE(v.GetData() == reinterpret_cast<std::int8_t*>(s.data()));
            REQUIRE(!view.IsEmpty());
        }
        SECTION("to std::int32_t with const") {
            auto v = view.ViewAs<std::int32_t const>();

            REQUIRE(v.GetSize() == 6);
            REQUIRE(v.GetBack() == 0x14325876);
            REQUIRE(v.GetData() == reinterpret_cast<std::int32_t*>(s.data()));
            REQUIRE(!view.IsEmpty());
        }
        SECTION("to std::int16_t with const") {
            auto v = view.ViewAs<std::int16_t const>();

            REQUIRE(v.GetSize() == 12);
            REQUIRE(v.GetBack() == 0x1432);
            REQUIRE(v.GetData() == reinterpret_cast<std::int16_t*>(s.data()));
            REQUIRE(!view.IsEmpty());
        }
        SECTION("to std::int8_t with const") {
            auto v = view.ViewAs<std::int8_t const>();

            REQUIRE(v.GetSize() == 24);
            REQUIRE(v.GetBack() == 0x14);
            REQUIRE(v.GetData() == reinterpret_cast<std::int8_t*>(s.data()));
            REQUIRE(!view.IsEmpty());
        }
    }
}
