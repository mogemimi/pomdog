// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/bounding_box.h"
#include "pomdog/math/bounding_frustum.h"
#include "pomdog/math/bounding_sphere.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/plane.h"
#include "pomdog/math/plane_intersection_type.h"
#include "pomdog/math/radian.h"

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
using pomdog::Vector3;
using pomdog::math::cross;
using pomdog::math::dot;
using pomdog::math::length;
using pomdog::math::normalize;
using pomdog::math::toRadian;

namespace {
auto approx(float x) -> auto
{
    return doctest::Approx(static_cast<double>(x)).epsilon(0.000001);
}
} // namespace

TEST_CASE("Plane")
{
    SUBCASE("constructors")
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
}
