// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/utility/string_hash64.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::computeStringHash64;
using pomdog::u64;

TEST_CASE("string_hash64")
{
    static_assert(computeStringHash64("") == u64(10525815183674050875ULL));
    static_assert(computeStringHash64("Hello") == u64(3119102237357052223ULL));
}
