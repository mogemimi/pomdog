// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/BoundingFrustum.hpp"
#include "Pomdog/Math/BoundingBox.hpp"
#include "Pomdog/Math/BoundingSphere.hpp"
#include "Pomdog/Math/ContainmentType.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Plane.hpp"
#include "Pomdog/Math/Radian.hpp"
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
