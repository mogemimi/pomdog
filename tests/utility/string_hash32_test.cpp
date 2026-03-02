// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/utility/string_hash32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::computeStringHash32;
using pomdog::u32;

TEST_CASE("string_hash32")
{
    static_assert(computeStringHash32("") == u32(3775933984UL));
    static_assert(computeStringHash32("Hello") == u32(2645664716UL));
}
