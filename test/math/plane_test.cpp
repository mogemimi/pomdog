// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_box.hpp"
#include "pomdog/math/bounding_frustum.hpp"
#include "pomdog/math/bounding_sphere.hpp"
#include "pomdog/math/plane.hpp"
#include "pomdog/math/plane_intersection_type.hpp"
#include "pomdog/math/ray.hpp"
#include <catch_amalgamated.hpp>

using namespace pomdog;

namespace {

constexpr auto Epsilon = std::numeric_limits<float>::epsilon();
constexpr auto Epsilon2 = 0.0000001f;

} // namespace

TEST_CASE("Plane", "[Plane]")
{
    SECTION("Constructor_Vector3_Float")
    {
        Plane plane{Vector3{1.0f, 2.0f, 3.0f}, 4.0f};
        REQUIRE(plane.Normal.X == 1.0f);
        REQUIRE(plane.Normal.Y == 2.0f);
        REQUIRE(plane.Normal.Z == 3.0f);
        REQUIRE(plane.Distance == 4.0f);
    }
    SECTION("Constructor_Vector3Array_1")
    {
        // NOTE: Left-handed coordinate system (not right-handed one)
        const Vector3 a = {4.0f, 2.0f, 3.0f};
        const Vector3 b = {4.0f, 7.0f, 3.0f};
        const Vector3 c = {8.0f, 2.0f, 3.0f};
        const Plane plane{a, b, c};

        {
            const auto ab = b - a;
            const auto ac = c - a;
            const auto normal = math::Normalize(math::Cross(ab, ac));
            const auto distance = -(math::Dot(normal, a));
            REQUIRE(std::abs(plane.Normal.X - normal.X) < Epsilon2);
            REQUIRE(std::abs(plane.Normal.Y - normal.Y) < Epsilon2);
            REQUIRE(std::abs(plane.Normal.Z - normal.Z) < Epsilon2);
            REQUIRE(std::abs(plane.Distance - distance) < Epsilon2);
        }
        {
            const auto bc = c - b;
            const auto ba = a - b;
            const auto normal = math::Normalize(math::Cross(bc, ba));
            const auto distance = -(math::Dot(normal, b));
            REQUIRE(std::abs(plane.Normal.X - normal.X) < Epsilon2);
            REQUIRE(std::abs(plane.Normal.Y - normal.Y) < Epsilon2);
            REQUIRE(std::abs(plane.Normal.Z - normal.Z) < Epsilon2);
            REQUIRE(std::abs(plane.Distance - distance) < Epsilon2);
        }
        {
            const auto ca = a - c;
            const auto cb = b - c;
            const auto normal = math::Normalize(math::Cross(ca, cb));
            const auto distance = -(math::Dot(normal, c));
            REQUIRE(std::abs(plane.Normal.X - normal.X) < Epsilon2);
            REQUIRE(std::abs(plane.Normal.Y - normal.Y) < Epsilon2);
            REQUIRE(std::abs(plane.Normal.Z - normal.Z) < Epsilon2);
            REQUIRE(std::abs(plane.Distance - distance) < Epsilon2);
        }
    }
    SECTION("Constructor_Vector3Array_2")
    {
        // NOTE: Left-handed coordinate system (not right-handed one)
        const Vector3 a = {-42.0f, -5.0f, 3.0f};
        const Vector3 b = {-42.0f, 81.0f, 3.0f};
        const Vector3 c = {73.0f, -4.0f, 3.0f};
        const Plane plane{a, b, c};

        {
            const auto ab = b - a;
            const auto ac = c - a;
            const auto normal = math::Normalize(math::Cross(ab, ac));
            const auto distance = -(math::Dot(normal, a));
            REQUIRE(std::abs(plane.Normal.X - normal.X) < Epsilon2);
            REQUIRE(std::abs(plane.Normal.Y - normal.Y) < Epsilon2);
            REQUIRE(std::abs(plane.Normal.Z - normal.Z) < Epsilon2);
            REQUIRE(std::abs(plane.Distance - distance) < Epsilon2);
        }
        {
            const auto bc = c - b;
            const auto ba = a - b;
            const auto normal = math::Normalize(math::Cross(bc, ba));
            const auto distance = -(math::Dot(normal, b));
            REQUIRE(std::abs(plane.Normal.X - normal.X) < Epsilon2);
            REQUIRE(std::abs(plane.Normal.Y - normal.Y) < Epsilon2);
            REQUIRE(std::abs(plane.Normal.Z - normal.Z) < Epsilon2);
            REQUIRE(std::abs(plane.Distance - distance) < Epsilon2);
        }
        {
            const auto ca = a - c;
            const auto cb = b - c;
            const auto normal = math::Normalize(math::Cross(ca, cb));
            const auto distance = -(math::Dot(normal, c));
            REQUIRE(std::abs(plane.Normal.X - normal.X) < Epsilon2);
            REQUIRE(std::abs(plane.Normal.Y - normal.Y) < Epsilon2);
            REQUIRE(std::abs(plane.Normal.Z - normal.Z) < Epsilon2);
            REQUIRE(std::abs(plane.Distance - distance) < Epsilon2);
        }
    }
    SECTION("Normalize_Zero")
    {
        Plane plane{Vector3::Zero(), 0.0f};
        plane.Normalize();
        REQUIRE(plane.Normal == Vector3{0.0f, 0.0f, 0.0f});
        REQUIRE(plane.Distance == 0.0f);
    }
    SECTION("Normalize_Epsilon")
    {
        {
            Plane plane{Vector3{Epsilon, 0.0f, 0.0f}, 0.0f};
            plane.Normalize();
            REQUIRE(plane.Normal == Vector3{1.0f, 0.0f, 0.0f});
            REQUIRE(plane.Distance == 0.0f / Epsilon);
        }
        {
            Plane plane{Vector3{0.0f, Epsilon, 0.0f}, 3.0f};
            plane.Normalize();
            REQUIRE(plane.Normal == Vector3{0.0f, 1.0f, 0.0f});
            REQUIRE(plane.Distance == 3.0f / Epsilon);
        }
        {
            Plane plane{Vector3{0.0f, 0.0f, Epsilon}, 42.0f};
            plane.Normalize();
            REQUIRE(plane.Normal == Vector3{0.0f, 0.0f, 1.0f});
            REQUIRE(plane.Distance == 42.0f / Epsilon);
        }
    }
    SECTION("Normalize_Factor")
    {
        const auto normal = Vector3{10.0f, 20.0f, 30.0f};
        const auto distance = 42.0f;

        Plane plane{normal, distance};
        plane.Normalize();

        const auto factor = math::Length(math::Normalize(normal)) / math::Length(normal);
        REQUIRE(plane.Normal == normal * factor);
        REQUIRE(plane.Distance == distance * factor);
    }
    SECTION("Intersects_Vector3_UnitX")
    {
        Plane plane{Vector3::UnitX(), 0.0f};
        REQUIRE(PlaneIntersectionType::Front == plane.Intersects(Vector3{1.0f, 0.0f, 0.0f}));
        REQUIRE(PlaneIntersectionType::Front == plane.Intersects(Vector3{1.0f, 42.0f, 42.0f}));
        REQUIRE(PlaneIntersectionType::Front == plane.Intersects(Vector3{1.0f, 42.0f, -42.0f}));
        REQUIRE(PlaneIntersectionType::Front == plane.Intersects(Vector3{1.0f, -42.0f, 42.0f}));
        REQUIRE(PlaneIntersectionType::Front == plane.Intersects(Vector3{1.0f, -42.0f, -42.0f}));
        REQUIRE(PlaneIntersectionType::Front == plane.Intersects(Vector3{42.0f, 0.0f, 0.0f}));
        REQUIRE(PlaneIntersectionType::Intersecting == plane.Intersects(Vector3{0.0f, 0.0f, 0.0f}));
        REQUIRE(PlaneIntersectionType::Intersecting == plane.Intersects(Vector3{0.0f, 42.0f, 0.0f}));
        REQUIRE(PlaneIntersectionType::Intersecting == plane.Intersects(Vector3{0.0f, 0.0f, 42.0f}));
        REQUIRE(PlaneIntersectionType::Intersecting == plane.Intersects(Vector3{0.0f, 42.0f, 87.0f}));
        REQUIRE(PlaneIntersectionType::Intersecting == plane.Intersects(Vector3{0.0f, -42.0f, -87.0f}));
        REQUIRE(PlaneIntersectionType::Back == plane.Intersects(Vector3{-1.0f, 0.0f, 0.0f}));
        REQUIRE(PlaneIntersectionType::Back == plane.Intersects(Vector3{-1.0f, 42.0f, 42.0f}));
        REQUIRE(PlaneIntersectionType::Back == plane.Intersects(Vector3{-1.0f, 42.0f, -42.0f}));
        REQUIRE(PlaneIntersectionType::Back == plane.Intersects(Vector3{-1.0f, -42.0f, 42.0f}));
        REQUIRE(PlaneIntersectionType::Back == plane.Intersects(Vector3{-1.0f, -42.0f, -42.0f}));
        REQUIRE(PlaneIntersectionType::Back == plane.Intersects(Vector3{-42.0f, 0.0f, 0.0f}));
    }
    SECTION("Intersects_Vector3_UnitY")
    {
        Plane plane{Vector3::UnitY(), -42.0f};

        // NOTE:
        // Y
        // ^
        // |
        // ---------------- Plane
        // |         |
        // | UnitY   | Distance = -42.0f
        // |   ^     |
        // |   |     v
        // *---------------> X

        REQUIRE(PlaneIntersectionType::Front == plane.Intersects(Vector3{0.0f, 100.0f, 0.0f}));
        REQUIRE(PlaneIntersectionType::Front == plane.Intersects(Vector3{0.0f, 43.0f, 0.0f}));
        REQUIRE(PlaneIntersectionType::Intersecting == plane.Intersects(Vector3{0.0f, 42.0f, 0.0f}));
        REQUIRE(PlaneIntersectionType::Back == plane.Intersects(Vector3{0.0f, 41.0f, 0.0f}));
        REQUIRE(PlaneIntersectionType::Back == plane.Intersects(Vector3{0.0f, -100.0f, 0.0f}));
    }
}
