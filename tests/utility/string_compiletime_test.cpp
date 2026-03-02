// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/utility/string_compiletime.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

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
