// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/utility/string_hash32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::computeStringHash32;
using pomdog::u32;
using pomdog::detail::strlen_compiletime;

TEST_CASE("strlen_compiletime")
{
    static_assert(strlen_compiletime("") == 0);
    static_assert(strlen_compiletime("a") == 1);
    static_assert(strlen_compiletime("  ") == 2);
    static_assert(strlen_compiletime("ABC") == 3);
    static_assert(strlen_compiletime("1234") == 4);
    static_assert(strlen_compiletime("Hello") == 5);
}

TEST_CASE("string_hash32")
{

    static_assert(computeStringHash32("") == u32(3775933984UL));
    static_assert(computeStringHash32("Hello") == u32(2645664716UL));
}
