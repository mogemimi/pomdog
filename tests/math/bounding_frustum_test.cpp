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
#include <array>
#include <cmath>
#include <random>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::BoundingBox;
using pomdog::BoundingFrustum;
using pomdog::BoundingSphere;
using pomdog::ContainmentType;
using pomdog::f32;
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

[[nodiscard]] std::array<Plane, 6>
getPlanes(const BoundingFrustum& frustum)
{
    return {frustum.getNear(), frustum.getFar(), frustum.getLeft(), frustum.getRight(), frustum.getTop(), frustum.getBottom()};
}

/// Returns what BoundingFrustum::contains returns, computed plane by plane.
template <typename T>
[[nodiscard]] ContainmentType
containsByPlanes(const BoundingFrustum& frustum, const T& shape)
{
    bool intersects = false;
    for (const auto& plane : getPlanes(frustum)) {
        const auto type = plane.intersects(shape);
        if (type == PlaneIntersectionType::Front) {
            return ContainmentType::Disjoint;
        }
        if (type == PlaneIntersectionType::Intersecting) {
            intersects = true;
        }
    }
    return intersects ? ContainmentType::Intersects : ContainmentType::Contains;
}

[[nodiscard]] ContainmentType
containsPointByPlanes(const BoundingFrustum& frustum, const Vector3& point)
{
    for (const auto& plane : getPlanes(frustum)) {
        if (plane.dotCoordinate(point) > 0.0f) {
            return ContainmentType::Disjoint;
        }
    }
    return ContainmentType::Contains;
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
    SUBCASE("getCorners of an orthographic frustum")
    {
        // NOTE: The planes are x = -2, x = 2, y = -1, y = 1, z = 1, and z = 9,
        // and every value along the way is a small dyadic number, so the
        // corners are exact with or without fused multiply-add.
        const BoundingFrustum frustum{Matrix4x4::createOrthographicLH(4.0f, 2.0f, 1.0f, 9.0f)};
        const auto& corners = frustum.getCorners();
        const std::array<Vector3, 8> expected = {{
            {-2.0f, 1.0f, 1.0f},
            {2.0f, 1.0f, 1.0f},
            {2.0f, -1.0f, 1.0f},
            {-2.0f, -1.0f, 1.0f},
            {-2.0f, 1.0f, 9.0f},
            {2.0f, 1.0f, 9.0f},
            {2.0f, -1.0f, 9.0f},
            {-2.0f, -1.0f, 9.0f},
        }};
        for (std::size_t i = 0; i < expected.size(); ++i) {
            CAPTURE(i);
            REQUIRE(corners[i] == expected[i]);
        }
    }
    SUBCASE("setMatrix")
    {
        // NOTE: setMatrix replaces every plane, so the frustum matches one
        // constructed from the new matrix.
        BoundingFrustum frustum{Matrix4x4::createOrthographicLH(4.0f, 2.0f, 1.0f, 9.0f)};
        const auto view = Matrix4x4::createLookAtLH(Vector3{3, 4, 5}, Vector3{10, 2, 60}, Vector3{0, 1, 0});
        const auto projection = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(60.0f), 16.0f / 9.0f, 0.1f, 100.0f);
        frustum.setMatrix(view * projection);
        const BoundingFrustum expected{view * projection};

        const auto planes = getPlanes(frustum);
        const auto expectedPlanes = getPlanes(expected);
        for (std::size_t i = 0; i < planes.size(); ++i) {
            CAPTURE(i);
            REQUIRE(planes[i].normal == expectedPlanes[i].normal);
            REQUIRE(planes[i].distance == expectedPlanes[i].distance);
        }
        const auto corners = frustum.getCorners();
        const auto expectedCorners = expected.getCorners();
        for (std::size_t i = 0; i < corners.size(); ++i) {
            CAPTURE(i);
            REQUIRE(corners[i] == expectedCorners[i]);
        }
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

TEST_CASE("BoundingFrustum queries match the planes")
{
    // NOTE: BoundingFrustum may test several planes at once. Its answers must
    // match testing the planes one by one with Plane::intersects.
    const auto view = Matrix4x4::createLookAtLH(Vector3{3, 4, 5}, Vector3{10, 2, 60}, Vector3{0, 1, 0});
    const auto projection = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(60.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    const BoundingFrustum frustum{view * projection};

    std::mt19937 random{42};
    std::uniform_real_distribution<f32> position{-100.0f, 120.0f};
    std::uniform_real_distribution<f32> size{0.0f, 10.0f};

    std::vector<BoundingBox> boxes;
    std::vector<BoundingSphere> spheres;
    std::vector<Vector3> points;
    for (int i = 0; i < 2000; ++i) {
        const Vector3 center{position(random), position(random), position(random)};
        const Vector3 extent{size(random), size(random), size(random)};
        boxes.emplace_back(center - extent, center + extent);
        spheres.emplace_back(center, extent.x);
        points.push_back(center);
    }
    // NOTE: Boxes and spheres around the corners touch three planes at once.
    // Shapes exactly at a corner are left out: their answers depend on the last
    // bit of each n . p + d, and compilers may fuse the multiply and add of the
    // scalar Plane code (Clang does on AArch64) but not those of F32x4.
    for (const auto& corner : frustum.getCorners()) {
        boxes.emplace_back(corner - Vector3{1, 1, 1}, corner + Vector3{1, 1, 1});
        spheres.emplace_back(corner, 1.0f);
    }

    std::array<int, 3> counts = {};
    for (const auto& box : boxes) {
        const auto expected = containsByPlanes(frustum, box);
        REQUIRE(frustum.contains(box) == expected);
        REQUIRE(frustum.intersects(box) == (expected != ContainmentType::Disjoint));
        counts[static_cast<std::size_t>(expected)] += 1;
    }
    for (const auto& sphere : spheres) {
        const auto expected = containsByPlanes(frustum, sphere);
        REQUIRE(frustum.contains(sphere) == expected);
        REQUIRE(frustum.intersects(sphere) == (expected != ContainmentType::Disjoint));
    }
    for (const auto& point : points) {
        REQUIRE(frustum.contains(point) == containsPointByPlanes(frustum, point));
    }

    // NOTE: The boxes cover all three answers.
    REQUIRE(counts[static_cast<std::size_t>(ContainmentType::Contains)] > 0);
    REQUIRE(counts[static_cast<std::size_t>(ContainmentType::Disjoint)] > 0);
    REQUIRE(counts[static_cast<std::size_t>(ContainmentType::Intersects)] > 0);
}
