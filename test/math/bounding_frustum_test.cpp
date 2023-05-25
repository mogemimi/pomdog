// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_box.h"
#include "pomdog/math/bounding_frustum.h"
#include "pomdog/math/bounding_sphere.h"
#include "pomdog/math/containment_type.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/plane.h"
#include "pomdog/math/radian.h"
#include <catch_amalgamated.hpp>

using namespace pomdog;

namespace {

constexpr auto PI = 3.1415926535f;
//constexpr auto Epsilon = std::numeric_limits<float>::epsilon();
constexpr auto Epsilon2 = 0.0000001f;

} // namespace

TEST_CASE("BoundingFrustum", "[BoundingFrustum]")
{
    SECTION("constructs frustum from view-projection matrix")
    {
        constexpr auto width = 800.0f;
        constexpr auto height = 480.0f;
        constexpr auto zNear = 0.1f;
        constexpr auto zFar = 100.0f;
        const auto view = Matrix4x4::createTranslation(Vector3{3.0f, 4.0f, 7.0f});
        const auto projection = Matrix4x4::createPerspectiveFieldOfViewLH(PI / 4.0f, width / height, zNear, zFar);

        BoundingFrustum frustum(view * projection);
        REQUIRE(std::abs(-frustum.getNear().normal.x - frustum.getFar().normal.x) < Epsilon2);
        REQUIRE(std::abs(-frustum.getNear().normal.y - frustum.getFar().normal.y) < Epsilon2);
        REQUIRE(std::abs(-frustum.getNear().normal.z - frustum.getFar().normal.z) < Epsilon2);
    }
}
