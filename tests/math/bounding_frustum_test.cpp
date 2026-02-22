// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/bounding_box.h"
#include "pomdog/math/bounding_frustum.h"
#include "pomdog/math/bounding_sphere.h"
#include "pomdog/math/containment_type.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/plane.h"
#include "pomdog/math/plane_intersection_type.h"
#include "pomdog/math/radian.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::BoundingBox;
using pomdog::BoundingFrustum;
using pomdog::BoundingSphere;
using pomdog::ContainmentType;
using pomdog::Matrix4x4;
using pomdog::Plane;
using pomdog::PlaneIntersectionType;
using pomdog::Vector3;
using pomdog::math::toRadian;

namespace {
auto approx(float x) -> auto
{
    return doctest::Approx(static_cast<double>(x)).epsilon(0.000001);
}
} // namespace

TEST_CASE("BoundingFrustum")
{
    SUBCASE("constructor from view-projection matrix")
    {
        auto view = Matrix4x4::createLookAtLH(Vector3{10, 0, 0}, Vector3{0, 0, 0}, Vector3{0, 1, 0});
        auto projection = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(45.0f), 800.0f / 480.0f, 0.01f, 1000.0f);
        BoundingFrustum frustum(view * projection);

        REQUIRE(-frustum.getNear().normal.x == approx(frustum.getFar().normal.x));
        REQUIRE(-frustum.getNear().normal.y == approx(frustum.getFar().normal.y));
        REQUIRE(-frustum.getNear().normal.z == approx(frustum.getFar().normal.z));
    }
    SUBCASE("getCorners")
    {
        auto eye = Vector3{3.0f, 4.0f, 5.0f};
        auto target = eye + Vector3{0.0f, 0.0f, 70.0f};
        auto view = Matrix4x4::createLookAtLH(eye, target, Vector3{0.0f, 1.0f, 0.0f});
        auto projection = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(45.0f), 800.0f / 480.0f, 0.01f, 1000.0f);
        BoundingFrustum frustum(view * projection);
        const auto& corners = frustum.getCorners();
        REQUIRE(corners.size() == 8);

        for (const auto& corner : corners) {
            REQUIRE(std::isfinite(corner.x));
            REQUIRE(std::isfinite(corner.y));
            REQUIRE(std::isfinite(corner.z));
        }

        constexpr int NearLeftTop = 0;
        constexpr int NearRightTop = 1;
        constexpr int NearRightBottom = 2;
        constexpr int NearLeftBottom = 3;
        constexpr int FarLeftTop = 4;
        constexpr int FarRightTop = 5;
        constexpr int FarRightBottom = 6;
        constexpr int FarLeftBottom = 7;

        REQUIRE(corners[NearLeftTop].x < corners[NearRightTop].x);
        REQUIRE(corners[NearLeftTop].y > corners[NearLeftBottom].y);
        REQUIRE(corners[NearLeftTop].z < corners[FarLeftTop].z);

        REQUIRE(corners[NearRightTop].x > corners[NearLeftTop].x);
        REQUIRE(corners[NearRightTop].y > corners[NearRightBottom].y);
        REQUIRE(corners[NearRightTop].z < corners[FarRightTop].z);

        REQUIRE(corners[NearRightBottom].x > corners[NearLeftBottom].x);
        REQUIRE(corners[NearRightBottom].y < corners[NearRightTop].y);
        REQUIRE(corners[NearRightBottom].z < corners[FarRightBottom].z);

        REQUIRE(corners[NearLeftBottom].x < corners[NearRightBottom].x);
        REQUIRE(corners[NearLeftBottom].y < corners[NearLeftTop].y);
        REQUIRE(corners[NearLeftBottom].z < corners[FarLeftBottom].z);

        REQUIRE(corners[FarLeftTop].x < corners[FarRightTop].x);
        REQUIRE(corners[FarLeftTop].y > corners[FarLeftBottom].y);
        REQUIRE(corners[FarLeftTop].z > corners[NearLeftTop].z);

        REQUIRE(corners[FarRightTop].x > corners[FarLeftTop].x);
        REQUIRE(corners[FarRightTop].y > corners[FarRightBottom].y);
        REQUIRE(corners[FarRightTop].z > corners[NearRightTop].z);

        REQUIRE(corners[FarRightBottom].x > corners[FarLeftBottom].x);
        REQUIRE(corners[FarRightBottom].y < corners[FarRightTop].y);
        REQUIRE(corners[FarRightBottom].z > corners[NearRightBottom].z);

        REQUIRE(corners[FarLeftBottom].x < corners[FarRightBottom].x);
        REQUIRE(corners[FarLeftBottom].y < corners[FarLeftTop].y);
        REQUIRE(corners[FarLeftBottom].z > corners[NearLeftBottom].z);
    }
    SUBCASE("contains Vector3")
    {
        auto view = Matrix4x4::createLookAtLH(Vector3{10, 0, 0}, Vector3{0, 0, 0}, Vector3{0, 1, 0});
        auto projection = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(45.0f), 800.0f / 480.0f, 0.01f, 1000.0f);
        BoundingFrustum frustum(view * projection);

        REQUIRE(frustum.contains(Vector3{0.0f, 0.0f, 0.0f}) == ContainmentType::Contains);
        REQUIRE(frustum.contains(Vector3{9.0f, 0.0f, 0.0f}) == ContainmentType::Contains);

        REQUIRE(frustum.contains(Vector3{10.01f, 0.0f, 0.0f}) == ContainmentType::Disjoint);
        REQUIRE(frustum.contains(Vector3{-991.0f, 0.0f, 0.0f}) == ContainmentType::Disjoint);
    }
    SUBCASE("contains BoundingBox")
    {
        auto view = Matrix4x4::createLookAtLH(Vector3{10, 0, 0}, Vector3{0, 0, 0}, Vector3{0, 1, 0});
        auto projection = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(45.0f), 800.0f / 480.0f, 0.01f, 1000.0f);
        BoundingFrustum frustum(view * projection);

        // NOTE: Box fully inside the frustum
        REQUIRE(frustum.contains(BoundingBox{Vector3{-1, -1, -1}, Vector3{1, 1, 1}}) == ContainmentType::Contains);
        REQUIRE(frustum.contains(BoundingBox{Vector3{5, -0.5f, -0.5f}, Vector3{6, 0.5f, 0.5f}}) == ContainmentType::Contains);

        // NOTE: Box fully outside the frustum
        REQUIRE(frustum.contains(BoundingBox{Vector3{11, 0, 0}, Vector3{12, 1, 1}}) == ContainmentType::Disjoint);
        REQUIRE(frustum.contains(BoundingBox{Vector3{-1100, -1, -1}, Vector3{-1050, 1, 1}}) == ContainmentType::Disjoint);
        REQUIRE(frustum.contains(BoundingBox{Vector3{100, 100, 100}, Vector3{200, 200, 200}}) == ContainmentType::Disjoint);

        // NOTE: Box partially overlapping the frustum (straddling the near plane)
        REQUIRE(frustum.contains(BoundingBox{Vector3{9.5f, -0.1f, -0.1f}, Vector3{10.5f, 0.1f, 0.1f}}) == ContainmentType::Intersects);

        // NOTE: Box partially overlapping the frustum (straddling the far plane)
        REQUIRE(frustum.contains(BoundingBox{Vector3{-995, -1, -1}, Vector3{-985, 1, 1}}) == ContainmentType::Intersects);
    }
    SUBCASE("contains BoundingSphere")
    {
        auto view = Matrix4x4::createLookAtLH(Vector3{10, 0, 0}, Vector3{0, 0, 0}, Vector3{0, 1, 0});
        auto projection = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(45.0f), 800.0f / 480.0f, 0.01f, 1000.0f);
        BoundingFrustum frustum(view * projection);

        // NOTE: Sphere fully inside the frustum
        REQUIRE(frustum.contains(BoundingSphere{Vector3{0, 0, 0}, 0.5f}) == ContainmentType::Contains);
        REQUIRE(frustum.contains(BoundingSphere{Vector3{5, 0, 0}, 1.0f}) == ContainmentType::Contains);

        // NOTE: Sphere fully outside the frustum
        REQUIRE(frustum.contains(BoundingSphere{Vector3{12, 0, 0}, 0.5f}) == ContainmentType::Disjoint);
        REQUIRE(frustum.contains(BoundingSphere{Vector3{-1100, 0, 0}, 1.0f}) == ContainmentType::Disjoint);
        REQUIRE(frustum.contains(BoundingSphere{Vector3{100, 100, 100}, 1.0f}) == ContainmentType::Disjoint);

        // NOTE: Sphere partially overlapping the frustum (straddling the near plane)
        REQUIRE(frustum.contains(BoundingSphere{Vector3{10.0f, 0, 0}, 0.5f}) == ContainmentType::Intersects);

        // NOTE: Sphere partially overlapping the frustum (straddling the far plane)
        REQUIRE(frustum.contains(BoundingSphere{Vector3{-990, 0, 0}, 5.0f}) == ContainmentType::Intersects);
    }
    SUBCASE("contains BoundingFrustum")
    {
        auto view = Matrix4x4::createLookAtLH(Vector3{10, 0, 0}, Vector3{0, 0, 0}, Vector3{0, 1, 0});
        auto projection = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(45.0f), 800.0f / 480.0f, 0.01f, 1000.0f);
        BoundingFrustum frustum(view * projection);

        // NOTE: Same frustum: corners lie exactly on planes, so Intersects
        REQUIRE(frustum.contains(frustum) == ContainmentType::Intersects);

        // NOTE: Smaller frustum inside the larger one
        auto smallProjection = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(20.0f), 800.0f / 480.0f, 0.1f, 500.0f);
        BoundingFrustum smallFrustum(view * smallProjection);
        REQUIRE(frustum.contains(smallFrustum) == ContainmentType::Contains);

        // NOTE: Frustum far away should be disjoint
        auto farView = Matrix4x4::createLookAtLH(Vector3{1000, 0, 0}, Vector3{2000, 0, 0}, Vector3{0, 1, 0});
        BoundingFrustum farFrustum(farView * projection);
        REQUIRE(frustum.contains(farFrustum) == ContainmentType::Disjoint);

        // NOTE: Partially overlapping frustum
        auto shiftedView = Matrix4x4::createLookAtLH(Vector3{10, 0, 0}, Vector3{0, 0, 0}, Vector3{0, 1, 0});
        auto wideProjection = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(45.0f), 800.0f / 480.0f, 0.01f, 2000.0f);
        BoundingFrustum wideFrustum(shiftedView * wideProjection);
        REQUIRE(frustum.contains(wideFrustum) == ContainmentType::Intersects);
    }
    SUBCASE("intersects BoundingBox")
    {
        auto view = Matrix4x4::createLookAtLH(Vector3{10, 0, 0}, Vector3{0, 0, 0}, Vector3{0, 1, 0});
        auto projection = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(45.0f), 800.0f / 480.0f, 0.01f, 1000.0f);
        BoundingFrustum frustum(view * projection);

        // NOTE: Box inside the frustum
        REQUIRE(frustum.intersects(BoundingBox{Vector3{-1, -1, -1}, Vector3{1, 1, 1}}));

        // NOTE: Box partially overlapping the frustum
        REQUIRE(frustum.intersects(BoundingBox{Vector3{9.5f, -0.1f, -0.1f}, Vector3{10.5f, 0.1f, 0.1f}}));

        // NOTE: Box fully outside the frustum
        REQUIRE_FALSE(frustum.intersects(BoundingBox{Vector3{11, 0, 0}, Vector3{12, 1, 1}}));
        REQUIRE_FALSE(frustum.intersects(BoundingBox{Vector3{-1100, -1, -1}, Vector3{-1050, 1, 1}}));
        REQUIRE_FALSE(frustum.intersects(BoundingBox{Vector3{100, 100, 100}, Vector3{200, 200, 200}}));
    }
    SUBCASE("intersects BoundingSphere")
    {
        auto view = Matrix4x4::createLookAtLH(Vector3{10, 0, 0}, Vector3{0, 0, 0}, Vector3{0, 1, 0});
        auto projection = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(45.0f), 800.0f / 480.0f, 0.01f, 1000.0f);
        BoundingFrustum frustum(view * projection);

        // NOTE: Sphere inside the frustum
        REQUIRE(frustum.intersects(BoundingSphere{Vector3{0, 0, 0}, 0.5f}));

        // NOTE: Sphere partially overlapping the frustum
        REQUIRE(frustum.intersects(BoundingSphere{Vector3{10.0f, 0, 0}, 0.5f}));

        // NOTE: Sphere fully outside the frustum
        REQUIRE_FALSE(frustum.intersects(BoundingSphere{Vector3{12, 0, 0}, 0.5f}));
        REQUIRE_FALSE(frustum.intersects(BoundingSphere{Vector3{-1100, 0, 0}, 1.0f}));
        REQUIRE_FALSE(frustum.intersects(BoundingSphere{Vector3{100, 100, 100}, 1.0f}));
    }
    SUBCASE("intersects BoundingFrustum")
    {
        auto view = Matrix4x4::createLookAtLH(Vector3{10, 0, 0}, Vector3{0, 0, 0}, Vector3{0, 1, 0});
        auto projection = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(45.0f), 800.0f / 480.0f, 0.01f, 1000.0f);
        BoundingFrustum frustum(view * projection);

        // NOTE: Same frustum should intersect
        REQUIRE(frustum.intersects(frustum));

        // NOTE: Smaller frustum fully contained should intersect
        auto smallProjection = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(20.0f), 800.0f / 480.0f, 0.1f, 500.0f);
        BoundingFrustum smallFrustum(view * smallProjection);
        REQUIRE(frustum.intersects(smallFrustum));

        // NOTE: Frustum far away should not intersect
        auto farView = Matrix4x4::createLookAtLH(Vector3{1000, 0, 0}, Vector3{2000, 0, 0}, Vector3{0, 1, 0});
        BoundingFrustum farFrustum(farView * projection);
        REQUIRE_FALSE(frustum.intersects(farFrustum));

        // NOTE: Partially overlapping frustum
        auto wideProjection = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(45.0f), 800.0f / 480.0f, 0.01f, 2000.0f);
        BoundingFrustum wideFrustum(view * wideProjection);
        REQUIRE(frustum.intersects(wideFrustum));
    }
    SUBCASE("intersects Plane")
    {
        auto view = Matrix4x4::createLookAtLH(Vector3{10, 0, 0}, Vector3{0, 0, 0}, Vector3{0, 1, 0});
        auto projection = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(45.0f), 800.0f / 480.0f, 0.01f, 1000.0f);
        BoundingFrustum frustum(view * projection);

        // NOTE: Plane cutting through the middle of the frustum
        REQUIRE(frustum.intersects(Plane{Vector3{1, 0, 0}, 0.0f}) == PlaneIntersectionType::Intersecting);

        // NOTE: Horizontal plane cutting through the frustum
        REQUIRE(frustum.intersects(Plane{Vector3{0, 1, 0}, 0.0f}) == PlaneIntersectionType::Intersecting);

        // NOTE: All corners on the front side (dotCoordinate > 0 for all corners)
        REQUIRE(frustum.intersects(Plane{Vector3{1, 0, 0}, 1000.0f}) == PlaneIntersectionType::Front);

        // NOTE: All corners on the back side (dotCoordinate < 0 for all corners)
        REQUIRE(frustum.intersects(Plane{Vector3{1, 0, 0}, -20.0f}) == PlaneIntersectionType::Back);
    }
}
