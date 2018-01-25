// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Math/BoundingBox2D.hpp>
#include <Pomdog/Math/BoundingCircle.hpp>
#include <Pomdog/Math/ContainmentType.hpp>
#include <gtest/iutest_switch.hpp>

using namespace Pomdog;

TEST(BoundingBox2D, TrivialCase)
{
    EXPECT_EQ(4, BoundingBox2D::CornerCount);
}

TEST(BoundingBox2D, Constructors)
{
    BoundingBox2D box;

    BoundingBox2D box2;
    box2.Min = Vector2::Zero;
    box2.Max = Vector2{42.0f, 3.14f};

    box = box2;
    EXPECT_EQ(Vector2::Zero, box.Min);
    EXPECT_EQ(Vector2(42.0f, 3.14f), box.Max);

    BoundingBox2D box3 {Vector2{-1.0f, 2.718f}, Vector2{17.0f, 8.0f}};
    box = box3;
    EXPECT_EQ(Vector2(-1.0f, 2.718f), box.Min);
    EXPECT_EQ(Vector2(-1.0f, 2.718f), box3.Min);
    EXPECT_EQ(Vector2(17.0f, 8.0f), box.Max);
    EXPECT_EQ(Vector2(17.0f, 8.0f), box3.Max);
}

TEST(BoundingBox2D, GetCorners)
{
    BoundingBox2D box;
    box.Min = Vector2{12, 13};
    box.Max = Vector2{15, 16};

    auto corners = box.GetCorners();
    ASSERT_FALSE(corners.empty());
    ASSERT_EQ(BoundingBox2D::CornerCount, corners.size());
    ASSERT_EQ(Vector2(12, 13), box.Min);
    ASSERT_EQ(Vector2(15, 16), box.Max);
    EXPECT_EQ(Vector2(box.Min.X, box.Max.Y), corners[0]);
    EXPECT_EQ(Vector2(box.Max.X, box.Max.Y), corners[1]);
    EXPECT_EQ(Vector2(box.Max.X, box.Min.Y), corners[2]);
    EXPECT_EQ(Vector2(box.Min.X, box.Min.Y), corners[3]);
}

TEST(BoundingBox2D, Contains_Vector3)
{
    constexpr auto a = 12.0f;
    constexpr auto b = 16.0f;
    constexpr auto d = 32.0f;
    constexpr auto e = 64.0f;

    BoundingBox2D box;
    box.Min = Vector2{a, b};
    box.Max = Vector2{d, e};

    EXPECT_EQ(ContainmentType::Contains, box.Contains(Vector2(a + 1, b + 1)));
    EXPECT_EQ(ContainmentType::Contains, box.Contains(Vector2(d - 1, b + 1)));
    EXPECT_EQ(ContainmentType::Contains, box.Contains(Vector2(a + 1, e - 1)));
    EXPECT_EQ(ContainmentType::Contains, box.Contains(Vector2(d - 1, e - 1)));

    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector2(a, b)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector2(d, b)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector2(a, e)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector2(d, e)));

    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector2((d + a) / 2, b)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector2((d + a) / 2, e)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector2(a, (e + b) / 2)));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(Vector2(d, (e + b) / 2)));

    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector2(a - 1, b)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector2(d + 1, b)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector2(a, b - 1)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector2(d, e + 1)));

    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector2(a - 1, b - 1)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector2(d + 1, e + 1)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector2(a - 1, e + 1)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector2(d + 1, b - 1)));

    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector2(a - 1, b - 1)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector2(d + 1, e + 1)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector2(a - 1, e + 1)));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(Vector2(d + 1, b - 1)));
}

TEST(BoundingBox2D, Contains_BoundingBox2D)
{
    constexpr auto a = 12.0f;
    constexpr auto b = 16.0f;
    constexpr auto d = 32.0f;
    constexpr auto e = 64.0f;

    BoundingBox2D box;
    box.Min = Vector2{a, b};
    box.Max = Vector2{d, e};

    EXPECT_EQ(ContainmentType::Contains, box.Contains(box));

    EXPECT_EQ(ContainmentType::Contains, box.Contains(BoundingBox2D{box.Min, Vector2::Lerp(box.Min, box.Max, 0.01f)}));
    EXPECT_EQ(ContainmentType::Contains, box.Contains(BoundingBox2D{box.Min, Vector2::Lerp(box.Min, box.Max, 0.5f)}));
    EXPECT_EQ(ContainmentType::Contains, box.Contains(BoundingBox2D{box.Min, Vector2::Lerp(box.Min, box.Max, 0.99f)}));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(BoundingBox2D{box.Min, Vector2::Lerp(box.Min, box.Max, 1.1f)}));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(BoundingBox2D{Vector2::Zero, Vector2(a, b)}));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(BoundingBox2D{Vector2::Zero, Vector2(a - 1, b)}));
}

TEST(BoundingBox2D, Contains_BoundingCircle)
{
    BoundingBox2D box;
    box.Min = Vector2{12.0f, 16.0f};
    box.Max = Vector2{32.0f, 64.0f};
    auto center = Vector2::Lerp(box.Min, box.Max, 0.5f);

    EXPECT_EQ(ContainmentType::Contains, box.Contains(BoundingCircle{center, 1.0f}));
    EXPECT_EQ(ContainmentType::Contains, box.Contains(BoundingCircle{center, (box.Max - box.Min).X / 2}));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(BoundingCircle{center, (box.Max - box.Min).Y / 2}));

    EXPECT_EQ(ContainmentType::Intersects, box.Contains(BoundingCircle{Vector2{0.0f, 16.0f}, 12.01f}));
    EXPECT_EQ(ContainmentType::Intersects, box.Contains(BoundingCircle{Vector2{12.0f, 0.0f,}, 16.01f}));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(BoundingCircle{Vector2{0.0f, 16.0f}, 11.99f}));
    EXPECT_EQ(ContainmentType::Disjoint, box.Contains(BoundingCircle{Vector2{12.0f, 0.0f}, 15.99f}));
}

TEST(BoundingBox2D, Intersects_BoundingBox2D)
{
    constexpr auto a = 12.0f;
    constexpr auto b = 16.0f;
    constexpr auto d = 32.0f;
    constexpr auto e = 64.0f;

    BoundingBox2D box;
    box.Min = Vector2{a, b};
    box.Max = Vector2{d, e};

    EXPECT_TRUE(box.Intersects(box));
    EXPECT_TRUE(box.Intersects(BoundingBox2D{box.Min, Vector2::Lerp(box.Min, box.Max, 0.01f)}));
    EXPECT_TRUE(box.Intersects(BoundingBox2D{box.Min, Vector2::Lerp(box.Min, box.Max, 0.5f)}));
    EXPECT_TRUE(box.Intersects(BoundingBox2D{box.Min, Vector2::Lerp(box.Min, box.Max, 0.99f)}));
    EXPECT_TRUE(box.Intersects(BoundingBox2D{box.Min, Vector2::Lerp(box.Min, box.Max, 1.1f)}));
    EXPECT_TRUE(box.Intersects(BoundingBox2D{Vector2::Zero, Vector2(a, b)}));
    EXPECT_FALSE(box.Intersects(BoundingBox2D{Vector2::Zero, Vector2(a - 1, b)}));
}

TEST(BoundingBox2D, Intersects_BoundingCircle)
{
    BoundingBox2D box;
    box.Min = Vector2{12.0f, 16.0f};
    box.Max = Vector2{32.0f, 64.0f};
    auto center = Vector2::Lerp(box.Min, box.Max, 0.5f);

    EXPECT_TRUE(box.Intersects(BoundingCircle{center, 1.0f}));
    EXPECT_TRUE(box.Intersects(BoundingCircle{center, (box.Max - box.Min).X / 2}));
    EXPECT_TRUE(box.Intersects(BoundingCircle{center, (box.Max - box.Min).Y / 2}));

    EXPECT_TRUE(box.Intersects(BoundingCircle{Vector2{0.0f, 16.0f}, 12.01f}));
    EXPECT_TRUE(box.Intersects(BoundingCircle{Vector2{12.0f, 0.0f}, 16.01f}));
    EXPECT_FALSE(box.Intersects(BoundingCircle{Vector2{0.0f, 16.0f}, 11.99f}));
    EXPECT_FALSE(box.Intersects(BoundingCircle{Vector2{12.0f, 0.0f}, 15.99f}));
}
