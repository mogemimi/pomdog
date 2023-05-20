// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/quaternion.h"
#include <catch_amalgamated.hpp>

using pomdog::Quaternion;

TEST_CASE("Quaternion", "[Quaternion]")
{
    SECTION("Quaternion")
    {
        Quaternion quaternion{0, 0, 0, 1};
        REQUIRE(quaternion.x == 0.0f);
        REQUIRE(quaternion.y == 0.0f);
        REQUIRE(quaternion.z == 0.0f);
        REQUIRE(quaternion.w == 1.0f);

        quaternion = {4, 3, 2, 1};
        REQUIRE(quaternion.x == 4.0f);
        REQUIRE(quaternion.y == 3.0f);
        REQUIRE(quaternion.z == 2.0f);
        REQUIRE(quaternion.w == 1.0f);
    }
    SECTION("Constants")
    {
        REQUIRE(Quaternion(0.0f, 0.0f, 0.0f, 1.0f) == Quaternion::Identity());
    }
    SECTION("operator/=")
    {
        REQUIRE((Quaternion(1.0f, 2.0f, 3.0f, 4.0f) / 5.0f) == Quaternion{1.0f / 5.0f, 2.0f / 5.0f, 3.0f / 5.0f, 4.0f / 5.0f});
    }
}
