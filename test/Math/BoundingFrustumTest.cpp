// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_frustum.hpp"
#include "pomdog/math/bounding_box.hpp"
#include "pomdog/math/bounding_sphere.hpp"
#include "pomdog/math/containment_type.hpp"
#include "pomdog/math/matrix4x4.hpp"
#include "pomdog/math/plane.hpp"
#include "pomdog/math/radian.hpp"
#include "catch_amalgamated.hpp"

using namespace Pomdog;

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
        const auto view = Matrix4x4::CreateTranslation(Vector3{3.0f, 4.0f, 7.0f});
        const auto projection = Matrix4x4::CreatePerspectiveFieldOfViewLH(PI / 4.0f, width / height, zNear, zFar);

        BoundingFrustum frustum(view * projection);
        REQUIRE(std::abs(-frustum.GetNear().Normal.X - frustum.GetFar().Normal.X) < Epsilon2);
        REQUIRE(std::abs(-frustum.GetNear().Normal.Y - frustum.GetFar().Normal.Y) < Epsilon2);
        REQUIRE(std::abs(-frustum.GetNear().Normal.Z - frustum.GetFar().Normal.Z) < Epsilon2);
    }
}
