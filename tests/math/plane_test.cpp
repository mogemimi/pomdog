// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/bounding_box.h"
#include "pomdog/math/bounding_frustum.h"
#include "pomdog/math/bounding_sphere.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/plane.h"
#include "pomdog/math/plane_intersection_type.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/turn.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::BoundingBox;
using pomdog::BoundingFrustum;
using pomdog::BoundingSphere;
using pomdog::Matrix4x4;
using pomdog::Plane;
using pomdog::PlaneIntersectionType;
using pomdog::Turn32;
using pomdog::Vector3;
using pomdog::math::cross;
using pomdog::math::dot;
using pomdog::math::length;
using pomdog::math::normalize;
using pomdog::math::toRadian;
using pomdog::math::transform;
using pomdog::math::transformNormal;

namespace {
auto approx(float x) -> auto
{
    return doctest::Approx(static_cast<double>(x)).epsilon(0.000001);
}
} // namespace

TEST_CASE("Plane")
{
    SUBCASE("constructor")
    {
        {
            Plane plane = {};
            REQUIRE(plane.normal.x == 0.0f);
            REQUIRE(plane.normal.y == 0.0f);
            REQUIRE(plane.normal.z == 0.0f);
            REQUIRE(plane.distance == 0.0f);
        }
        {
            Plane plane = {};
            plane.normal = Vector3{1.0f, 2.0f, 3.0f};
            plane.distance = 4.0f;
            REQUIRE(plane.normal.x == 1.0f);
            REQUIRE(plane.normal.y == 2.0f);
            REQUIRE(plane.normal.z == 3.0f);
            REQUIRE(plane.distance == 4.0f);
        }
        {
            Plane plane{Vector3{1.0f, 2.0f, 3.0f}, 4.0f};
            REQUIRE(plane.normal.x == 1.0f);
            REQUIRE(plane.normal.y == 2.0f);
            REQUIRE(plane.normal.z == 3.0f);
            REQUIRE(plane.distance == 4.0f);
        }
    }
    SUBCASE("constructor from three points")
    {
        // NOTE: Left-handed coordinate system (not right-handed one)
        const Vector3 a = {4.0f, 2.0f, 3.0f};
        const Vector3 b = {4.0f, 7.0f, 3.0f};
        const Vector3 c = {8.0f, 2.0f, 3.0f};

        Plane plane{a, b, c};

        {
            const auto ab = b - a;
            const auto ac = c - a;
            const auto normal = normalize(cross(ab, ac));
            const auto distance = -(dot(normal, a));
            REQUIRE(plane.normal.x == approx(normal.x));
            REQUIRE(plane.normal.y == approx(normal.y));
            REQUIRE(plane.normal.z == approx(normal.z));
            REQUIRE(plane.distance == approx(distance));
        }
        {
            const auto bc = c - b;
            const auto ba = a - b;
            const auto normal = normalize(cross(bc, ba));
            const auto distance = -(dot(normal, b));
            REQUIRE(plane.normal.x == approx(normal.x));
            REQUIRE(plane.normal.y == approx(normal.y));
            REQUIRE(plane.normal.z == approx(normal.z));
            REQUIRE(plane.distance == approx(distance));
        }
        {
            const auto ca = a - c;
            const auto cb = b - c;
            const auto normal = normalize(cross(ca, cb));
            const auto distance = -(dot(normal, c));
            REQUIRE(plane.normal.x == approx(normal.x));
            REQUIRE(plane.normal.y == approx(normal.y));
            REQUIRE(plane.normal.z == approx(normal.z));
            REQUIRE(plane.distance == approx(distance));
        }
    }
    SUBCASE("constructor from three points with large coordinates")
    {
        // NOTE: Left-handed coordinate system (not right-handed one)
        const Vector3 a = {-42.0f, -5.0f, 3.0f};
        const Vector3 b = {-42.0f, 81.0f, 3.0f};
        const Vector3 c = {73.0f, -4.0f, 3.0f};

        Plane plane{a, b, c};

        {
            const auto ab = b - a;
            const auto ac = c - a;
            const auto normal = normalize(cross(ab, ac));
            const auto distance = -(dot(normal, a));
            REQUIRE(plane.normal.x == approx(normal.x));
            REQUIRE(plane.normal.y == approx(normal.y));
            REQUIRE(plane.normal.z == approx(normal.z));
            REQUIRE(plane.distance == approx(distance));
        }
        {
            const auto bc = c - b;
            const auto ba = a - b;
            const auto normal = normalize(cross(bc, ba));
            const auto distance = -(dot(normal, b));
            REQUIRE(plane.normal.x == approx(normal.x));
            REQUIRE(plane.normal.y == approx(normal.y));
            REQUIRE(plane.normal.z == approx(normal.z));
            REQUIRE(plane.distance == approx(distance));
        }
        {
            const auto ca = a - c;
            const auto cb = b - c;
            const auto normal = normalize(cross(ca, cb));
            const auto distance = -(dot(normal, c));
            REQUIRE(plane.normal.x == approx(normal.x));
            REQUIRE(plane.normal.y == approx(normal.y));
            REQUIRE(plane.normal.z == approx(normal.z));
            REQUIRE(plane.distance == approx(distance));
        }
    }
    SUBCASE("normalize")
    {
        {
            auto plane = Plane::normalize(Plane{Vector3{0.0f, 0.0f, 1.0f}, 0.0f});
            REQUIRE(plane.normal.x == approx(0.0f));
            REQUIRE(plane.normal.y == approx(0.0f));
            REQUIRE(plane.normal.z == approx(1.0f));
            REQUIRE(plane.distance == approx(0.0f));
        }
        {
            auto plane = Plane::normalize(Plane{Vector3{0.0f, 0.0f, 2.0f}, 0.0f});
            REQUIRE(plane.normal.x == approx(0.0f));
            REQUIRE(plane.normal.y == approx(0.0f));
            REQUIRE(plane.normal.z == approx(1.0f));
            REQUIRE(plane.distance == approx(0.0f));
        }
        {
            auto plane = Plane::normalize(Plane{Vector3{0.0f, 0.0f, 2.0f}, 4.0f});
            REQUIRE(plane.normal.x == approx(0.0f));
            REQUIRE(plane.normal.y == approx(0.0f));
            REQUIRE(plane.normal.z == approx(1.0f));
            REQUIRE(plane.distance == approx(2.0f));
        }
    }
    SUBCASE("normalize with zero normal")
    {
        constexpr auto epsilon = std::numeric_limits<float>::epsilon();

        {
            Plane plane{Vector3::createZero(), 0.0f};
            plane.normalize();
            REQUIRE(plane.normal == Vector3{0.0f, 0.0f, 0.0f});
            REQUIRE(plane.distance == 0.0f);
        }
        {
            Plane plane{Vector3::createZero(), 42.0f};
            plane.normalize();
            REQUIRE(plane.normal == Vector3{0.0f, 0.0f, 0.0f});
            REQUIRE(plane.distance == 42.0f);
        }
        {
            Plane plane{Vector3{epsilon, 0.0f, 0.0f}, 0.0f};
            plane.normalize();
            REQUIRE(plane.normal == Vector3{1.0f, 0.0f, 0.0f});
            REQUIRE(plane.distance == 0.0f / epsilon);
        }
        {
            Plane plane{Vector3{0.0f, epsilon, 0.0f}, 3.0f};
            plane.normalize();
            REQUIRE(plane.normal == Vector3{0.0f, 1.0f, 0.0f});
            REQUIRE(plane.distance == 3.0f / epsilon);
        }
        {
            Plane plane{Vector3{0.0f, 0.0f, epsilon}, 42.0f};
            plane.normalize();
            REQUIRE(plane.normal == Vector3{0.0f, 0.0f, 1.0f});
            REQUIRE(plane.distance == 42.0f / epsilon);
        }
    }
    SUBCASE("normalize factor")
    {
        const auto normal = Vector3{10.0f, 20.0f, 30.0f};
        const auto distance = 42.0f;

        Plane plane{normal, distance};
        plane.normalize();

        const auto factor = length(normalize(normal)) / length(normal);
        REQUIRE(plane.normal == normal * factor);
        REQUIRE(plane.distance == distance * factor);
    }
    SUBCASE("dotCoordinate")
    {
        REQUIRE(Plane{Vector3{0.0f, 0.0f, 1.0f}, 0.0f}.dotCoordinate(Vector3{0.0f, 0.0f, 0.0f}) == approx(0.0f));
        REQUIRE(Plane{Vector3{0.0f, 0.0f, 1.0f}, 0.0f}.dotCoordinate(Vector3{0.0f, 0.0f, 1.0f}) == approx(1.0f));
        REQUIRE(Plane{Vector3{0.0f, 0.0f, 1.0f}, 0.0f}.dotCoordinate(Vector3{0.0f, 0.0f, -1.0f}) == approx(-1.0f));
        REQUIRE(Plane{Vector3{0.0f, 0.0f, 1.0f}, 0.0f}.dotCoordinate(Vector3{0.0f, 0.0f, 5.0f}) == approx(5.0f));
        REQUIRE(Plane{Vector3{0.0f, 0.0f, 1.0f}, 3.0f}.dotCoordinate(Vector3{0.0f, 0.0f, 5.0f}) == approx(8.0f));
    }
    SUBCASE("dotNormal")
    {
        REQUIRE(Plane{Vector3{0.0f, 0.0f, 1.0f}, 0.0f}.dotNormal(Vector3{0.0f, 0.0f, 0.0f}) == approx(0.0f));
        REQUIRE(Plane{Vector3{0.0f, 0.0f, 1.0f}, 0.0f}.dotNormal(Vector3{0.0f, 0.0f, 1.0f}) == approx(1.0f));
        REQUIRE(Plane{Vector3{0.0f, 0.0f, 1.0f}, 0.0f}.dotNormal(Vector3{0.0f, 0.0f, -1.0f}) == approx(-1.0f));
        REQUIRE(Plane{Vector3{0.0f, 0.0f, 1.0f}, 0.0f}.dotNormal(Vector3{0.0f, 0.0f, 5.0f}) == approx(5.0f));
        REQUIRE(Plane{Vector3{0.0f, 0.0f, 1.0f}, 3.0f}.dotNormal(Vector3{0.0f, 0.0f, 5.0f}) == approx(5.0f));
    }
    SUBCASE("intersects Vector3::createUnitX()")
    {
        Plane plane{Vector3::createUnitX(), 0.0f};
        REQUIRE(plane.intersects(Vector3{1.0f, 0.0f, 0.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{1.0f, 42.0f, 42.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{1.0f, 42.0f, -42.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{1.0f, -42.0f, 42.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{1.0f, -42.0f, -42.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{42.0f, 0.0f, 0.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{0.0f, 0.0f, 0.0f}) == PlaneIntersectionType::Intersecting);
        REQUIRE(plane.intersects(Vector3{0.0f, 42.0f, 0.0f}) == PlaneIntersectionType::Intersecting);
        REQUIRE(plane.intersects(Vector3{0.0f, 0.0f, 42.0f}) == PlaneIntersectionType::Intersecting);
        REQUIRE(plane.intersects(Vector3{0.0f, 42.0f, 87.0f}) == PlaneIntersectionType::Intersecting);
        REQUIRE(plane.intersects(Vector3{0.0f, -42.0f, -87.0f}) == PlaneIntersectionType::Intersecting);
        REQUIRE(plane.intersects(Vector3{-1.0f, 0.0f, 0.0f}) == PlaneIntersectionType::Back);
        REQUIRE(plane.intersects(Vector3{-1.0f, 42.0f, 42.0f}) == PlaneIntersectionType::Back);
        REQUIRE(plane.intersects(Vector3{-1.0f, 42.0f, -42.0f}) == PlaneIntersectionType::Back);
        REQUIRE(plane.intersects(Vector3{-1.0f, -42.0f, 42.0f}) == PlaneIntersectionType::Back);
        REQUIRE(plane.intersects(Vector3{-1.0f, -42.0f, -42.0f}) == PlaneIntersectionType::Back);
        REQUIRE(plane.intersects(Vector3{-42.0f, 0.0f, 0.0f}) == PlaneIntersectionType::Back);
    }
    SUBCASE("intersects Vector3::createUnitY()")
    {
        Plane plane{Vector3::createUnitY(), 0.0f};
        REQUIRE(plane.intersects(Vector3{0.0f, 1.0f, 0.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{42.0f, 1.0f, 42.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{42.0f, 1.0f, -42.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{-42.0f, 1.0f, 42.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{-42.0f, 1.0f, -42.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{0.0f, 42.0f, 0.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{0.0f, 0.0f, 0.0f}) == PlaneIntersectionType::Intersecting);
        REQUIRE(plane.intersects(Vector3{42.0f, 0.0f, 42.0f}) == PlaneIntersectionType::Intersecting);
        REQUIRE(plane.intersects(Vector3{42.0f, 0.0f, -42.0f}) == PlaneIntersectionType::Intersecting);
        REQUIRE(plane.intersects(Vector3{-42.0f, 0.0f, 42.0f}) == PlaneIntersectionType::Intersecting);
        REQUIRE(plane.intersects(Vector3{-42.0f, 0.0f, -42.0f}) == PlaneIntersectionType::Intersecting);
        REQUIRE(plane.intersects(Vector3{1.0f, -1.0f, 1.0f}) == PlaneIntersectionType::Back);
        REQUIRE(plane.intersects(Vector3{1.0f, -1.0f, -1.0f}) == PlaneIntersectionType::Back);
        REQUIRE(plane.intersects(Vector3{-1.0f, -1.0f, 1.0f}) == PlaneIntersectionType::Back);
        REQUIRE(plane.intersects(Vector3{-1.0f, -1.0f, -1.0f}) == PlaneIntersectionType::Back);
        REQUIRE(plane.intersects(Vector3{0.0f, -1.0f, 0.0f}) == PlaneIntersectionType::Back);
        REQUIRE(plane.intersects(Vector3{0.0f, -42.0f, 0.0f}) == PlaneIntersectionType::Back);
    }
    SUBCASE("intersects Vector3::createUnitZ()")
    {
        Plane plane{Vector3::createUnitZ(), 0.0f};
        REQUIRE(plane.intersects(Vector3{0.0f, 0.0f, 1.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{42.0f, 42.0f, 1.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{42.0f, -42.0f, 1.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{-42.0f, 42.0f, 1.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{-42.0f, -42.0f, 1.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{0.0f, 0.0f, 42.0f}) == PlaneIntersectionType::Front);
        REQUIRE(plane.intersects(Vector3{0.0f, 0.0f, 0.0f}) == PlaneIntersectionType::Intersecting);
        REQUIRE(plane.intersects(Vector3{42.0f, 42.0f, 0.0f}) == PlaneIntersectionType::Intersecting);
        REQUIRE(plane.intersects(Vector3{42.0f, -42.0f, 0.0f}) == PlaneIntersectionType::Intersecting);
        REQUIRE(plane.intersects(Vector3{-42.0f, 42.0f, 0.0f}) == PlaneIntersectionType::Intersecting);
        REQUIRE(plane.intersects(Vector3{-42.0f, -42.0f, 0.0f}) == PlaneIntersectionType::Intersecting);
        REQUIRE(plane.intersects(Vector3{1.0f, 1.0f, -1.0f}) == PlaneIntersectionType::Back);
        REQUIRE(plane.intersects(Vector3{1.0f, -1.0f, -1.0f}) == PlaneIntersectionType::Back);
        REQUIRE(plane.intersects(Vector3{-1.0f, 1.0f, -1.0f}) == PlaneIntersectionType::Back);
        REQUIRE(plane.intersects(Vector3{-1.0f, -1.0f, -1.0f}) == PlaneIntersectionType::Back);
        REQUIRE(plane.intersects(Vector3{0.0f, 0.0f, -1.0f}) == PlaneIntersectionType::Back);
        REQUIRE(plane.intersects(Vector3{0.0f, 0.0f, -42.0f}) == PlaneIntersectionType::Back);
    }
    SUBCASE("intersects BoundingBox")
    {
        BoundingBox box;
        box.min = Vector3{-1.0f, -1.0f, -1.0f};
        box.max = Vector3{1.0f, 1.0f, 1.0f};

        Plane plane;

        plane = Plane{Vector3{0.0f, 0.0f, 1.0f}, 0.0f};
        REQUIRE(plane.intersects(box) == PlaneIntersectionType::Intersecting);

        plane = Plane{Vector3{0.0f, 0.0f, 1.0f}, 1.0f};
        REQUIRE(plane.intersects(box) == PlaneIntersectionType::Intersecting);

        plane = Plane{Vector3{0.0f, 0.0f, 1.0f}, -1.0f};
        REQUIRE(plane.intersects(box) == PlaneIntersectionType::Intersecting);

        plane = Plane{Vector3{0.0f, 0.0f, 1.0f}, 1.1f};
        REQUIRE(plane.intersects(box) == PlaneIntersectionType::Front);

        plane = Plane{Vector3{0.0f, 0.0f, 1.0f}, -1.1f};
        REQUIRE(plane.intersects(box) == PlaneIntersectionType::Back);
    }
    SUBCASE("intersects BoundingSphere")
    {
        BoundingSphere sphere;
        sphere.center = Vector3{0.0f, 0.0f, 0.0f};
        sphere.radius = 1.0f;

        Plane plane;

        plane = Plane{Vector3{0.0f, 0.0f, 1.0f}, 0.0f};
        REQUIRE(plane.intersects(sphere) == PlaneIntersectionType::Intersecting);

        plane = Plane{Vector3{0.0f, 0.0f, 1.0f}, 0.9f};
        REQUIRE(plane.intersects(sphere) == PlaneIntersectionType::Intersecting);

        plane = Plane{Vector3{0.0f, 0.0f, 1.0f}, -0.9f};
        REQUIRE(plane.intersects(sphere) == PlaneIntersectionType::Intersecting);

        plane = Plane{Vector3{0.0f, 0.0f, 1.0f}, 1.1f};
        REQUIRE(plane.intersects(sphere) == PlaneIntersectionType::Front);

        plane = Plane{Vector3{0.0f, 0.0f, 1.0f}, -1.1f};
        REQUIRE(plane.intersects(sphere) == PlaneIntersectionType::Back);
    }
    // SUBCASE("intersects BoundingFrustum") - skipped due to BoundingFrustum layout differences
    SUBCASE("transform")
    {
        // NOTE: Plane::transform goes through math::invert, so allow more error than approx.
        const auto approxTransformed = [](float x) {
            return doctest::Approx(static_cast<double>(x)).epsilon(0.00001);
        };

        // NOTE: Requires the point `point * matrix` to keep the signed distance of `point`
        // to `plane`. Under a transform that is not rigid, only points on `plane` keep it.
        const auto requireDistanceKept = [&](const Plane& plane, const Matrix4x4& matrix, const Vector3& point) {
            const auto result = Plane::transform(plane, matrix);
            REQUIRE(result.dotCoordinate(transform(point, matrix)) == approxTransformed(plane.dotCoordinate(point)));
        };

        // The plane y = 2.
        const auto planeY2 = Plane::createFromPointNormal(Vector3{0.0f, 2.0f, 0.0f}, Vector3::createUnitY());

        const Vector3 a = {1.0f, 2.0f, 3.0f};
        const Vector3 b = {-2.0f, 4.0f, 1.0f};
        const Vector3 c = {3.0f, -1.0f, 5.0f};
        const auto tilted = Plane{a, b, c};

        {
            // Translation
            const auto matrix = Matrix4x4::createTranslation(Vector3{3.0f, 5.0f, -1.0f});
            const auto result = Plane::transform(planeY2, matrix);
            REQUIRE(result.normal.x == approx(0.0f));
            REQUIRE(result.normal.y == approx(1.0f));
            REQUIRE(result.normal.z == approx(0.0f));
            REQUIRE(result.distance == approx(-7.0f));

            requireDistanceKept(planeY2, matrix, Vector3{0.0f, 2.0f, 0.0f});
            requireDistanceKept(planeY2, matrix, Vector3{4.0f, 2.0f, -3.0f});
            requireDistanceKept(planeY2, matrix, Vector3{1.0f, 5.0f, 2.0f});
            requireDistanceKept(planeY2, matrix, Vector3{-2.0f, -1.0f, 4.0f});
        }
        {
            // Rotation
            const auto matrix = Matrix4x4::createRotationZ(Turn32{0.125f});
            const auto result = Plane::transform(planeY2, matrix);
            const auto normal = transformNormal(planeY2.normal, matrix);
            REQUIRE(result.normal.x == approxTransformed(normal.x));
            REQUIRE(result.normal.y == approxTransformed(normal.y));
            REQUIRE(result.normal.z == approxTransformed(normal.z));
            REQUIRE(result.distance == approxTransformed(-2.0f));

            requireDistanceKept(planeY2, matrix, Vector3{0.0f, 2.0f, 0.0f});
            requireDistanceKept(planeY2, matrix, Vector3{4.0f, 2.0f, -3.0f});
            requireDistanceKept(planeY2, matrix, Vector3{1.0f, 5.0f, 2.0f});
            requireDistanceKept(planeY2, matrix, Vector3{-2.0f, -1.0f, 4.0f});
        }
        {
            // Rotation and translation
            const auto matrix = Matrix4x4::createFromAxisAngle(normalize(Vector3{1.0f, 2.0f, 3.0f}), Turn32{0.3f}) *
                                Matrix4x4::createTranslation(Vector3{3.0f, 5.0f, -1.0f});
            const auto result = Plane::transform(tilted, matrix);
            const auto normal = transformNormal(tilted.normal, matrix);
            REQUIRE(result.normal.x == approxTransformed(normal.x));
            REQUIRE(result.normal.y == approxTransformed(normal.y));
            REQUIRE(result.normal.z == approxTransformed(normal.z));

            requireDistanceKept(tilted, matrix, a);
            requireDistanceKept(tilted, matrix, b);
            requireDistanceKept(tilted, matrix, c);
            requireDistanceKept(tilted, matrix, a + tilted.normal * 3.0f);
            requireDistanceKept(tilted, matrix, b - tilted.normal * 2.0f);
        }
        {
            // Non-uniform scale, rotation, and translation
            const auto matrix = Matrix4x4::createScale(Vector3{2.0f, 0.5f, 4.0f}) *
                                Matrix4x4::createRotationX(Turn32{0.2f}) *
                                Matrix4x4::createTranslation(Vector3{3.0f, 5.0f, -1.0f});
            requireDistanceKept(tilted, matrix, a);
            requireDistanceKept(tilted, matrix, b);
            requireDistanceKept(tilted, matrix, c);
        }
        {
            // NOTE: Every element of the matrix, its inverse, and the plane is a
            // short dyadic fraction, so every product and sum is exact and the
            // result is exact with or without fused multiply-add.
            //
            //   inverse = | 0.5    0     0   0 |
            //             | 0      0.25  0   0 |
            //             | 0      0     2   0 |
            //             | -1.5   0.5  -2   1 |
            const auto matrix = Matrix4x4::createScale(Vector3{2.0f, 4.0f, 0.5f}) *
                                Matrix4x4::createTranslation(Vector3{3.0f, -2.0f, 1.0f});
            const auto result = Plane::transform(Plane{Vector3{0.5f, 0.25f, -1.0f}, 2.0f}, matrix);
            REQUIRE(result.normal.x == 0.25f);
            REQUIRE(result.normal.y == 0.0625f);
            REQUIRE(result.normal.z == -2.0f);
            REQUIRE(result.distance == 3.375f);
        }
    }
}
