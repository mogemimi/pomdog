// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Math/Plane.hpp>
#include <Pomdog/Math/BoundingBox.hpp>
#include <Pomdog/Math/BoundingFrustum.hpp>
#include <Pomdog/Math/BoundingSphere.hpp>
#include <Pomdog/Math/PlaneIntersectionType.hpp>
#include <Pomdog/Math/Ray.hpp>
#include <gtest/iutest_switch.hpp>

using namespace Pomdog;

namespace {

constexpr auto Epsilon = std::numeric_limits<float>::epsilon();
constexpr auto Epsilon2 = 0.0000001f;

} // namespace

TEST(Plane, Constructor_Vector3_Float)
{
    Plane plane{Vector3{1.0f, 2.0f, 3.0f}, 4.0f};
    EXPECT_EQ(plane.Normal.X, 1.0f);
    EXPECT_EQ(plane.Normal.Y, 2.0f);
    EXPECT_EQ(plane.Normal.Z, 3.0f);
    EXPECT_EQ(plane.Distance, 4.0f);
}

TEST(Plane, Constructor_Vector3Array_1)
{
    // NOTE: Left-handed coordinate system (not right-handed one)
    const Vector3 a = {4.0f, 2.0f, 3.0f};
    const Vector3 b = {4.0f, 7.0f, 3.0f};
    const Vector3 c = {8.0f, 2.0f, 3.0f};
    const Plane plane{a, b, c};

    {
        const auto ab = b - a;
        const auto ac = c - a;
        const auto normal = Vector3::Normalize(Vector3::Cross(ab, ac));
        const auto distance = -(Vector3::Dot(normal, a));
        EXPECT_NEAR(plane.Normal.X, normal.X, Epsilon2);
        EXPECT_NEAR(plane.Normal.Y, normal.Y, Epsilon2);
        EXPECT_NEAR(plane.Normal.Z, normal.Z, Epsilon2);
        EXPECT_NEAR(plane.Distance, distance, Epsilon2);
    }
    {
        const auto bc = c - b;
        const auto ba = a - b;
        const auto normal = Vector3::Normalize(Vector3::Cross(bc, ba));
        const auto distance = -(Vector3::Dot(normal, b));
        EXPECT_NEAR(plane.Normal.X, normal.X, Epsilon2);
        EXPECT_NEAR(plane.Normal.Y, normal.Y, Epsilon2);
        EXPECT_NEAR(plane.Normal.Z, normal.Z, Epsilon2);
        EXPECT_NEAR(plane.Distance, distance, Epsilon2);
    }
    {
        const auto ca = a - c;
        const auto cb = b - c;
        const auto normal = Vector3::Normalize(Vector3::Cross(ca, cb));
        const auto distance = -(Vector3::Dot(normal, c));
        EXPECT_NEAR(plane.Normal.X, normal.X, Epsilon2);
        EXPECT_NEAR(plane.Normal.Y, normal.Y, Epsilon2);
        EXPECT_NEAR(plane.Normal.Z, normal.Z, Epsilon2);
        EXPECT_NEAR(plane.Distance, distance, Epsilon2);
    }
}

TEST(Plane, Constructor_Vector3Array_2)
{
    // NOTE: Left-handed coordinate system (not right-handed one)
    const Vector3 a = {-42.0f, -5.0f, 3.0f};
    const Vector3 b = {-42.0f, 81.0f, 3.0f};
    const Vector3 c = {73.0f, -4.0f, 3.0f};
    const Plane plane{a, b, c};

    {
        const auto ab = b - a;
        const auto ac = c - a;
        const auto normal = Vector3::Normalize(Vector3::Cross(ab, ac));
        const auto distance = -(Vector3::Dot(normal, a));
        EXPECT_NEAR(plane.Normal.X, normal.X, Epsilon2);
        EXPECT_NEAR(plane.Normal.Y, normal.Y, Epsilon2);
        EXPECT_NEAR(plane.Normal.Z, normal.Z, Epsilon2);
        EXPECT_NEAR(plane.Distance, distance, Epsilon2);
    }
    {
        const auto bc = c - b;
        const auto ba = a - b;
        const auto normal = Vector3::Normalize(Vector3::Cross(bc, ba));
        const auto distance = -(Vector3::Dot(normal, b));
        EXPECT_NEAR(plane.Normal.X, normal.X, Epsilon2);
        EXPECT_NEAR(plane.Normal.Y, normal.Y, Epsilon2);
        EXPECT_NEAR(plane.Normal.Z, normal.Z, Epsilon2);
        EXPECT_NEAR(plane.Distance, distance, Epsilon2);
    }
    {
        const auto ca = a - c;
        const auto cb = b - c;
        const auto normal = Vector3::Normalize(Vector3::Cross(ca, cb));
        const auto distance = -(Vector3::Dot(normal, c));
        EXPECT_NEAR(plane.Normal.X, normal.X, Epsilon2);
        EXPECT_NEAR(plane.Normal.Y, normal.Y, Epsilon2);
        EXPECT_NEAR(plane.Normal.Z, normal.Z, Epsilon2);
        EXPECT_NEAR(plane.Distance, distance, Epsilon2);
    }
}

TEST(Plane, Normalize_Zero)
{
    Plane plane{Vector3::Zero, 0.0f};
    plane.Normalize();
    EXPECT_EQ(plane.Normal, Vector3(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(plane.Distance, 0.0f);
}

TEST(Plane, Normalize_Epsilon)
{
    {
        Plane plane{Vector3{Epsilon, 0.0f, 0.0f}, 0.0f};
        plane.Normalize();
        EXPECT_EQ(plane.Normal, Vector3(1.0f, 0.0f, 0.0f));
        EXPECT_EQ(plane.Distance, 0.0f / Epsilon);
    }
    {
        Plane plane{Vector3{0.0f, Epsilon, 0.0f}, 3.0f};
        plane.Normalize();
        EXPECT_EQ(plane.Normal, Vector3(0.0f, 1.0f, 0.0f));
        EXPECT_EQ(plane.Distance, 3.0f / Epsilon);
    }
    {
        Plane plane{Vector3{0.0f, 0.0f, Epsilon}, 42.0f};
        plane.Normalize();
        EXPECT_EQ(plane.Normal, Vector3(0.0f, 0.0f, 1.0f));
        EXPECT_EQ(plane.Distance, 42.0f / Epsilon);
    }
}

TEST(Plane, Normalize_Factor)
{
    const auto normal = Vector3{10.0f, 20.0f, 30.0f};
    const auto distance = 42.0f;

    Plane plane{normal, distance};
    plane.Normalize();

    const auto factor = Vector3::Normalize(normal).Length() / normal.Length();
    EXPECT_EQ(plane.Normal, normal * factor);
    EXPECT_EQ(plane.Distance, distance * factor);
}

TEST(Plane, Intersects_Vector3_UnitX)
{
    Plane plane{Vector3::UnitX, 0.0f};
    EXPECT_EQ(PlaneIntersectionType::Front, plane.Intersects(Vector3{1.0f, 0.0f, 0.0f}));
    EXPECT_EQ(PlaneIntersectionType::Front, plane.Intersects(Vector3{1.0f, 42.0f, 42.0f}));
    EXPECT_EQ(PlaneIntersectionType::Front, plane.Intersects(Vector3{1.0f, 42.0f, -42.0f}));
    EXPECT_EQ(PlaneIntersectionType::Front, plane.Intersects(Vector3{1.0f, -42.0f, 42.0f}));
    EXPECT_EQ(PlaneIntersectionType::Front, plane.Intersects(Vector3{1.0f, -42.0f, -42.0f}));
    EXPECT_EQ(PlaneIntersectionType::Front, plane.Intersects(Vector3{42.0f, 0.0f, 0.0f}));
    EXPECT_EQ(PlaneIntersectionType::Intersecting, plane.Intersects(Vector3{0.0f, 0.0f, 0.0f}));
    EXPECT_EQ(PlaneIntersectionType::Intersecting, plane.Intersects(Vector3{0.0f, 42.0f, 0.0f}));
    EXPECT_EQ(PlaneIntersectionType::Intersecting, plane.Intersects(Vector3{0.0f, 0.0f, 42.0f}));
    EXPECT_EQ(PlaneIntersectionType::Intersecting, plane.Intersects(Vector3{0.0f, 42.0f, 87.0f}));
    EXPECT_EQ(PlaneIntersectionType::Intersecting, plane.Intersects(Vector3{0.0f, -42.0f, -87.0f}));
    EXPECT_EQ(PlaneIntersectionType::Back, plane.Intersects(Vector3{-1.0f, 0.0f, 0.0f}));
    EXPECT_EQ(PlaneIntersectionType::Back, plane.Intersects(Vector3{-1.0f, 42.0f, 42.0f}));
    EXPECT_EQ(PlaneIntersectionType::Back, plane.Intersects(Vector3{-1.0f, 42.0f, -42.0f}));
    EXPECT_EQ(PlaneIntersectionType::Back, plane.Intersects(Vector3{-1.0f, -42.0f, 42.0f}));
    EXPECT_EQ(PlaneIntersectionType::Back, plane.Intersects(Vector3{-1.0f, -42.0f, -42.0f}));
    EXPECT_EQ(PlaneIntersectionType::Back, plane.Intersects(Vector3{-42.0f, 0.0f, 0.0f}));
}

TEST(Plane, Intersects_Vector3_UnitY)
{
    Plane plane{Vector3::UnitY, -42.0f};

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

    EXPECT_EQ(PlaneIntersectionType::Front, plane.Intersects(Vector3{0.0f, 100.0f, 0.0f}));
    EXPECT_EQ(PlaneIntersectionType::Front, plane.Intersects(Vector3{0.0f, 43.0f, 0.0f}));
    EXPECT_EQ(PlaneIntersectionType::Intersecting, plane.Intersects(Vector3{0.0f, 42.0f, 0.0f}));
    EXPECT_EQ(PlaneIntersectionType::Back, plane.Intersects(Vector3{0.0f, 41.0f, 0.0f}));
    EXPECT_EQ(PlaneIntersectionType::Back, plane.Intersects(Vector3{0.0f, -100.0f, 0.0f}));
}
