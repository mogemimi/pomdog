// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Math/BoundingBox.hpp>
#include <Pomdog/Math/BoundingSphere.hpp>
#include <Pomdog/Math/ContainmentType.hpp>
#include <gtest/iutest_switch.hpp>

using namespace Pomdog;

TEST(BoundingBox, TrivialCase)
{
    EXPECT_EQ(8, BoundingBox::CornerCount);
}

TEST(BoundingBox, Constructors)
{
    BoundingBox box;

    BoundingBox box2;
    box2.Min = Vector3::Zero;
    box2.Max = Vector3::UnitY;

    box = box2;
    EXPECT_EQ(Vector3::Zero, box.Min);
    EXPECT_EQ(Vector3::UnitY, box.Max);

    BoundingBox box3 {Vector3::UnitX, Vector3::UnitZ};
    box = box3;
    EXPECT_EQ(Vector3::UnitX, box.Min);
    EXPECT_EQ(Vector3::UnitX, box3.Min);
    EXPECT_EQ(Vector3::UnitZ, box.Max);
    EXPECT_EQ(Vector3::UnitZ, box3.Max);
}

TEST(BoundingBox, GetCorners)
{
    BoundingBox box;
    box.Min = Vector3{12, 13, 14};
    box.Max = Vector3{15, 16, 17};

    auto corners = box.GetCorners();
    ASSERT_FALSE(corners.empty());
    ASSERT_EQ(BoundingBox::CornerCount, corners.size());
    ASSERT_EQ(Vector3(12, 13, 14), box.Min);
    ASSERT_EQ(Vector3(15, 16, 17), box.Max);
    EXPECT_EQ(Vector3(box.Min.X, box.Max.Y, box.Max.Z), corners[0]);
    EXPECT_EQ(Vector3(box.Max.X, box.Max.Y, box.Max.Z), corners[1]);
    EXPECT_EQ(Vector3(box.Max.X, box.Min.Y, box.Max.Z), corners[2]);
    EXPECT_EQ(Vector3(box.Min.X, box.Min.Y, box.Max.Z), corners[3]);
    EXPECT_EQ(Vector3(box.Min.X, box.Max.Y, box.Min.Z), corners[4]);
    EXPECT_EQ(Vector3(box.Max.X, box.Max.Y, box.Min.Z), corners[5]);
    EXPECT_EQ(Vector3(box.Max.X, box.Min.Y, box.Min.Z), corners[6]);
    EXPECT_EQ(Vector3(box.Min.X, box.Min.Y, box.Min.Z), corners[7]);
}

TEST(BoundingBox, Contains_Vector3)
{
    constexpr auto a = 12.0f;
    constexpr auto b = 16.0f;
    constexpr auto c = 24.0f;
    constexpr auto d = 32.0f;
    constexpr auto e = 64.0f;
    constexpr auto f = 96.0f;

    BoundingBox box;
    box.Min = Vector3{a, b, c};
    box.Max = Vector3{d, e, f};

    EXPECT_EQ(ContainmentType::Contains, box.Contains(Vector3(a + 1, b + 1, c + 1)));
    EXPECT_EQ(ContainmentType::Contains, box.Contains(Vector3(d - 1, b + 1, c + 1)));
    EXPECT_EQ(ContainmentType::Contains, box.Contains(Vector3(a + 1, e - 1, c + 1)));
    EXPECT_EQ(ContainmentType::Contains, box.Contains(Vector3(d - 1, e - 1, c + 1)));
    EXPECT_EQ(ContainmentType::Contains, box.Contains(Vector3(a + 1, b + 1, f - 1)));
    EXPECT_EQ(ContainmentType::Contains, box.Contains(Vector3(d - 1, b + 1, f - 1)));
    EXPECT_EQ(ContainmentType::Contains, box.Contains(Vector3(a + 1, e - 1, f - 1)));
    EXPECT_EQ(ContainmentType::Contains, box.Contains(Vector3(d - 1, e - 1, f - 1)));

    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3(a, b, c)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3(d, b, c)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3(a, e, c)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3(d, e, c)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3(a, b, f)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3(d, b, f)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3(a, e, f)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3(d, e, f)));

    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3((d + a) / 2, b, c)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3((d + a) / 2, b, f)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3((d + a) / 2, e, c)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3((d + a) / 2, e, f)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3(a, (e + b) / 2, c)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3(a, (e + b) / 2, f)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3(d, (e + b) / 2, c)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3(d, (e + b) / 2, f)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3(a, b, (f + c) / 2)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3(a, e, (f + c) / 2)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3(d, b, (f + c) / 2)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector3(d, e, (f + c) / 2)));

    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(a - 1, b, c)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(d + 1, b, c)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(a, b - 1, c)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(d, e + 1, c)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(a, b, c - 1)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(d, b, f + 1)));

    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(a - 1, b - 1, c)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(d + 1, e + 1, c)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(a - 1, e + 1, c)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(d + 1, b - 1, c)));

    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(a - 1, b - 1, c - 1)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(d + 1, e + 1, c - 1)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(a - 1, e + 1, c - 1)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(d + 1, b - 1, c - 1)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(a - 1, b - 1, f + 1)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(d + 1, e + 1, f + 1)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(a - 1, e + 1, f + 1)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector3(d + 1, b - 1, f + 1)));
}

TEST(BoundingBox, Contains_BoundingBox)
{
    constexpr auto a = 12.0f;
    constexpr auto b = 16.0f;
    constexpr auto c = 24.0f;
    constexpr auto d = 32.0f;
    constexpr auto e = 64.0f;
    constexpr auto f = 96.0f;

    BoundingBox box;
    box.Min = Vector3{a, b, c};
    box.Max = Vector3{d, e, f};

    EXPECT_EQ(ContainmentType::Contains, box.Contains(box));

    EXPECT_EQ(ContainmentType::Contains, box.Contains(BoundingBox{box.Min, Vector3::Lerp(box.Min, box.Max, 0.01f)}));
    EXPECT_EQ(ContainmentType::Contains, box.Contains(BoundingBox{box.Min, Vector3::Lerp(box.Min, box.Max, 0.5f)}));
    EXPECT_EQ(ContainmentType::Contains, box.Contains(BoundingBox{box.Min, Vector3::Lerp(box.Min, box.Max, 0.99f)}));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(BoundingBox{box.Min, Vector3::Lerp(box.Min, box.Max, 1.1f)}));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(BoundingBox{Vector3::Zero, Vector3(a, b, c)}));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(BoundingBox{Vector3::Zero, Vector3(a - 1, b, c)}));
}

TEST(BoundingBox, Contains_BoundingSphere)
{
    BoundingBox box;
    box.Min = Vector3{12.0f, 16.0f, 24.0f};
    box.Max = Vector3{32.0f, 64.0f, 96.0f};
    auto center = Vector3::Lerp(box.Min, box.Max, 0.5f);

    EXPECT_EQ(ContainmentType::Contains, box.Contains(BoundingSphere{center, 1.0f}));
    EXPECT_EQ(ContainmentType::Contains, box.Contains(BoundingSphere{center, (box.Max - box.Min).X / 2}));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(BoundingSphere{center, (box.Max - box.Min).Y / 2}));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(BoundingSphere{center, (box.Max - box.Min).Z / 2}));

    EXPECT_EQ(ContainmentType::Intersects, box.Contains(BoundingSphere{Vector3{0.0f, 16.0f, 24.0f}, 12.01f}));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(BoundingSphere{Vector3{12.0f, 0.0f, 24.0f}, 16.01f}));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(BoundingSphere{Vector3{12.0f, 16.0f, 0.0f}, 24.01f}));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(BoundingSphere{Vector3{0.0f, 16.0f, 24.0f}, 11.99f}));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(BoundingSphere{Vector3{12.0f, 0.0f, 24.0f}, 15.99f}));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(BoundingSphere{Vector3{12.0f, 16.0f, 0.0f}, 23.99f}));
}

TEST(BoundingBox, Intersects_BoundingBox)
{
    constexpr auto a = 12.0f;
    constexpr auto b = 16.0f;
    constexpr auto c = 24.0f;
    constexpr auto d = 32.0f;
    constexpr auto e = 64.0f;
    constexpr auto f = 96.0f;

    BoundingBox box;
    box.Min = Vector3{a, b, c};
    box.Max = Vector3{d, e, f};

    EXPECT_TRUE(box.Intersects(box));
    EXPECT_TRUE(box.Intersects(BoundingBox{box.Min, Vector3::Lerp(box.Min, box.Max, 0.01f)}));
    EXPECT_TRUE(box.Intersects(BoundingBox{box.Min, Vector3::Lerp(box.Min, box.Max, 0.5f)}));
    EXPECT_TRUE(box.Intersects(BoundingBox{box.Min, Vector3::Lerp(box.Min, box.Max, 0.99f)}));
    EXPECT_TRUE(box.Intersects(BoundingBox{box.Min, Vector3::Lerp(box.Min, box.Max, 1.1f)}));
    EXPECT_TRUE(box.Intersects(BoundingBox{Vector3::Zero, Vector3(a, b, c)}));
    EXPECT_FALSE(box.Intersects(BoundingBox{Vector3::Zero, Vector3(a - 1, b, c)}));
}

TEST(BoundingBox, Intersects_BoundingSphere)
{
    BoundingBox box;
    box.Min = Vector3{12.0f, 16.0f, 24.0f};
    box.Max = Vector3{32.0f, 64.0f, 96.0f};
    auto center = Vector3::Lerp(box.Min, box.Max, 0.5f);

    EXPECT_TRUE(box.Intersects(BoundingSphere{center, 1.0f}));
    EXPECT_TRUE(box.Intersects(BoundingSphere{center, (box.Max - box.Min).X / 2}));
    EXPECT_TRUE(box.Intersects(BoundingSphere{center, (box.Max - box.Min).Y / 2}));
    EXPECT_TRUE(box.Intersects(BoundingSphere{center, (box.Max - box.Min).Z / 2}));

    EXPECT_TRUE(box.Intersects(BoundingSphere{Vector3{0.0f, 16.0f, 24.0f}, 12.01f}));
    EXPECT_TRUE(box.Intersects(BoundingSphere{Vector3{12.0f, 0.0f, 24.0f}, 16.01f}));
    EXPECT_TRUE(box.Intersects(BoundingSphere{Vector3{12.0f, 16.0f, 0.0f}, 24.01f}));
    EXPECT_FALSE(box.Intersects(BoundingSphere{Vector3{0.0f, 16.0f, 24.0f}, 11.99f}));
    EXPECT_FALSE(box.Intersects(BoundingSphere{Vector3{12.0f, 0.0f, 24.0f}, 15.99f}));
    EXPECT_FALSE(box.Intersects(BoundingSphere{Vector3{12.0f, 16.0f, 0.0f}, 23.99f}));
}
