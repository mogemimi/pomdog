// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/utility/variant_index.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <variant>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::variant_index;

TEST_CASE("variant_index")
{
    using VariantType = std::variant<int, float, double, char>;
    static_assert(variant_index<VariantType, int>() == 0);
    static_assert(variant_index<VariantType, float>() == 1);
    static_assert(variant_index<VariantType, double>() == 2);
    static_assert(variant_index<VariantType, char>() == 3);
}
