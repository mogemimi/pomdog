// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/memory/raw_ptr.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

TEST_CASE("raw_ptr")
{
    static_assert(std::is_convertible_v<pomdog::raw_ptr<int>, int*>);
    static_assert(std::is_convertible_v<pomdog::raw_ptr<const int>, const int*>);

    static_assert(std::is_convertible_v<pomdog::raw_ptr<void>, void*>);
    static_assert(std::is_convertible_v<pomdog::raw_ptr<const void>, const void*>);
}
