// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/experimental/skeletal2d/joint_pose.h"
#include "pomdog/experimental/skeletal2d/skeleton_helper.h"
#include "pomdog/math/matrix3x2.h"
#include "pomdog/math/turn.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::f32;
using pomdog::i32;
using pomdog::Matrix3x2;
using pomdog::Turn32;
using pomdog::Vector2;
using pomdog::skeletal2d::JointPose;
namespace math = pomdog::math;
namespace SkeletonHelper = pomdog::skeletal2d::SkeletonHelper;

TEST_CASE("SkeletonHelper::toLocalMatrix")
{
    SUBCASE("quarter turn is exact")
    {
        const JointPose pose = {
            .translate = Vector2{3.0f, 4.0f},
            .rotation = Turn32{0.25f},
            .scale = 2.0f,
        };
        const auto matrix = SkeletonHelper::toLocalMatrix(pose);
        REQUIRE_EQ(matrix(0, 0), 0.0f);
        REQUIRE_EQ(matrix(0, 1), 2.0f);
        REQUIRE_EQ(matrix(1, 0), -2.0f);
        REQUIRE_EQ(matrix(1, 1), 0.0f);
        REQUIRE_EQ(matrix(2, 0), 3.0f);
        REQUIRE_EQ(matrix(2, 1), 4.0f);
    }
    SUBCASE("matches the product of scale, rotation, and translation matrices")
    {
        constexpr std::array<f32, 7> rotations = {{-0.5f, -0.3f, -0.125f, 0.0f, 0.1f, 0.375f, 0.5f}};
        for (const f32 rotation : rotations) {
            CAPTURE(rotation);
            const JointPose pose = {
                .translate = Vector2{-7.0f, 5.0f},
                .rotation = Turn32{rotation},
                .scale = 1.5f,
            };
            const auto expected =
                Matrix3x2::createScale(pose.scale) *
                Matrix3x2::createRotation(pose.rotation) *
                Matrix3x2::createTranslation(pose.translate);
            const auto actual = SkeletonHelper::toLocalMatrix(pose);
            for (i32 row = 0; row < 3; ++row) {
                for (i32 column = 0; column < 2; ++column) {
                    REQUIRE_EQ(actual(row, column), expected(row, column));
                }
            }
        }
    }
}
