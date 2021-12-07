// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/quaternion.hpp"
#include <catch_amalgamated.hpp>

using Pomdog::Quaternion;

TEST_CASE("Quaternion", "[Quaternion]")
{
    SECTION("Quaternion")
    {
        Quaternion quaternion{0, 0, 0, 1};
        REQUIRE(quaternion.X == 0.0f);
        REQUIRE(quaternion.Y == 0.0f);
        REQUIRE(quaternion.Z == 0.0f);
        REQUIRE(quaternion.W == 1.0f);

        quaternion = {4, 3, 2, 1};
        REQUIRE(quaternion.X == 4.0f);
        REQUIRE(quaternion.Y == 3.0f);
        REQUIRE(quaternion.Z == 2.0f);
        REQUIRE(quaternion.W == 1.0f);
    }
    SECTION("Constants")
    {
        REQUIRE(Quaternion(0.0f, 0.0f, 0.0f, 1.0f) == Quaternion::Identity);
    }
}
