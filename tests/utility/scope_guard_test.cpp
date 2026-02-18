// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/utility/scope_guard.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <type_traits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::ScopeGuard;

TEST_CASE("ScopeGuard")
{
    SUBCASE("execute on scope exit")
    {
        int value = 0;
        {
            ScopeGuard guard([&value] {
                value = 42;
            });
            REQUIRE(value == 0);
        }
        REQUIRE(value == 42);
    }
    SUBCASE("dismiss prevents execution")
    {
        int value = 0;
        {
            ScopeGuard guard([&value] {
                value = 42;
            });
            guard.dismiss();
            REQUIRE(value == 0);
        }
        REQUIRE(value == 0);
    }
    SUBCASE("multiple guards")
    {
        int value1 = 0;
        int value2 = 0;
        {
            ScopeGuard guard1([&value1] {
                value1 = 42;
            });
            ScopeGuard guard2([&value2] {
                value2 = 84;
            });
            REQUIRE(value1 == 0);
            REQUIRE(value2 == 0);
        }
        REQUIRE(value1 == 42);
        REQUIRE(value2 == 84);
    }
    SUBCASE("move semantics")
    {
        int value = 0;
        {
            ScopeGuard guard1([&value] {
                value = 42;
            });
            ScopeGuard guard2(std::move(guard1));
            REQUIRE(value == 0);
        }
        REQUIRE(value == 42);
    }
    SUBCASE("non-copyable and non-assignable")
    {
        ScopeGuard guard1([] {});
        static_assert(std::is_move_constructible_v<decltype(guard1)>);
        static_assert(!std::is_copy_constructible_v<decltype(guard1)>);
        static_assert(!std::is_copy_assignable_v<decltype(guard1)>);
        static_assert(!std::is_move_assignable_v<decltype(guard1)>);
    }
}
