// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

TEST_CASE("string_format")
{
    using pomdog::format;

    CHECK(format("Hello, {}!", "world") == "Hello, world!");
    CHECK(format("{} + {} = {}", 1, 2, 3) == "1 + 2 = 3");
    CHECK(format("{0} {1} {0}", "foo", "bar") == "foo bar foo");
}
