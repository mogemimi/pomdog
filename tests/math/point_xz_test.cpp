// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/point_xz.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector_xz.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::i32;
using pomdog::PointXZ;
using pomdog::Vector2;
using pomdog::VectorXZ;
namespace math = pomdog::math;

TEST_CASE("PointXZ")
{
    SUBCASE("constructor")
    {
        PointXZ coordinate{0, 0};
        REQUIRE(coordinate.x == 0);
        REQUIRE(coordinate.z == 0);

        coordinate = {1, 2};
        REQUIRE(coordinate.x == 1);
        REQUIRE(coordinate.z == 2);

        coordinate = {
            std::numeric_limits<i32>::max(),
            std::numeric_limits<i32>::min(),
        };
        REQUIRE(coordinate.x == std::numeric_limits<i32>::max());
        REQUIRE(coordinate.z == std::numeric_limits<i32>::min());
    }
    SUBCASE("add")
    {
        REQUIRE(PointXZ{0, 0} == PointXZ{0, 0} + PointXZ{0, 0});
        REQUIRE(PointXZ{7, 9} == PointXZ{4, 5} + PointXZ{3, 4});
    }
    SUBCASE("subtract")
    {
        REQUIRE(PointXZ{0, 0} == PointXZ{0, 0} - PointXZ{0, 0});
        REQUIRE(PointXZ{2 - 5, 3 - 4} == PointXZ{2, 3} - PointXZ{5, 4});
    }
    SUBCASE("multiply by scalar")
    {
        REQUIRE(PointXZ{0, 0} == PointXZ{0, 0} * 0);
        REQUIRE(PointXZ{0, 0} == PointXZ{7, 8} * 0);
        REQUIRE(PointXZ{0, 0} == PointXZ{0, 0} * 9);
        REQUIRE(PointXZ{63, 72} == PointXZ{7, 8} * 9);
    }
    SUBCASE("scalar multiply")
    {
        REQUIRE(PointXZ{0, 0} == 0 * PointXZ{0, 0});
        REQUIRE(PointXZ{0, 0} == 0 * PointXZ{7, 8});
        REQUIRE(PointXZ{0, 0} == 9 * PointXZ{0, 0});
        REQUIRE(PointXZ{63, 72} == 9 * PointXZ{7, 8});
    }
    SUBCASE("multiply pointwise")
    {
        REQUIRE(PointXZ{0, 0} == PointXZ{0, 0} * PointXZ{0, 0});
        REQUIRE(PointXZ{21, 72} == PointXZ{7, 8} * PointXZ{3, 9});
    }
    SUBCASE("divide by scalar")
    {
        REQUIRE(PointXZ{0, 0} == PointXZ{0, 0} / 1);
        REQUIRE(PointXZ{7, 8} == PointXZ{7, 8} / 1);
        REQUIRE(PointXZ{0, 0} == PointXZ{0, 0} / 9);
        REQUIRE(PointXZ{70 / 9, 80 / 9} == PointXZ{70, 80} / 9);
    }
    SUBCASE("divide pointwise")
    {
        REQUIRE(PointXZ{0, 0} == PointXZ{0, 0} / PointXZ{1, 1});
        REQUIRE(PointXZ{1, 1} == PointXZ{7, 8} / PointXZ{7, 8});
        REQUIRE(PointXZ{0, 0} == PointXZ{0, 0} / PointXZ{9, 9});
        REQUIRE(PointXZ{70 / 5, 80 / 10} == PointXZ{70, 80} / PointXZ{5, 10});
    }
    SUBCASE("toVector2")
    {
        REQUIRE(math::toVector2(PointXZ{6, 7}) == Vector2{6.0f, 7.0f});
        REQUIRE(math::toVector2(PointXZ{-6, -7}) == Vector2{-6.0f, -7.0f});
    }
    SUBCASE("toVectorXZ")
    {
        auto v = math::toVectorXZ(PointXZ{6, 7});
        REQUIRE(v.x == 6.0f);
        REQUIRE(v.z == 7.0f);
    }
    SUBCASE("abs")
    {
        REQUIRE(math::abs(PointXZ{6, 7}) == PointXZ{6, 7});
        REQUIRE(math::abs(PointXZ{-6, -7}) == PointXZ{6, 7});
        REQUIRE(math::abs(PointXZ{-6, 7}) == PointXZ{6, 7});
        REQUIRE(math::abs(PointXZ{6, -7}) == PointXZ{6, 7});
    }
    SUBCASE("min")
    {
        REQUIRE(math::min(PointXZ{1, 5}, PointXZ{3, 2}) == PointXZ{1, 2});
    }
    SUBCASE("max")
    {
        REQUIRE(math::max(PointXZ{1, 5}, PointXZ{3, 2}) == PointXZ{3, 5});
    }
    SUBCASE("clamp")
    {
        REQUIRE(math::clamp(PointXZ{5, 5}, PointXZ{0, 0}, PointXZ{10, 10}) == PointXZ{5, 5});
        REQUIRE(math::clamp(PointXZ{-1, 15}, PointXZ{0, 0}, PointXZ{10, 10}) == PointXZ{0, 10});
    }
}
