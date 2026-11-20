// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/matrix3x3.h"
#include "pomdog/math/quaternion.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/turn.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::f32;
using pomdog::i32;
using pomdog::Matrix3x3;
using pomdog::Quaternion;
using pomdog::Radian32;
using pomdog::Turn32;
using pomdog::Vector2;
using pomdog::Vector3;
using pomdog::math::multiply;

namespace {
auto approx(float x) -> auto
{
    return doctest::Approx(static_cast<double>(x)).epsilon(0.000001);
}
} // namespace

TEST_CASE("Matrix3x3")
{
    SUBCASE("constructor")
    {
        Matrix3x3 matrix{
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f};
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 1.0f);
        REQUIRE(matrix(0, 2) == 1.0f);
        REQUIRE(matrix(1, 0) == 1.0f);
        REQUIRE(matrix(1, 1) == 1.0f);
        REQUIRE(matrix(1, 2) == 1.0f);
        REQUIRE(matrix(2, 0) == 1.0f);
        REQUIRE(matrix(2, 1) == 1.0f);
        REQUIRE(matrix(2, 2) == 1.0f);

        matrix = Matrix3x3{
            0.0f, 1.0f, 2.0f,
            3.0f, 4.0f, 5.0f,
            6.0f, 7.0f, 8.0f};
        REQUIRE(matrix(0, 0) == 0.0f);
        REQUIRE(matrix(0, 1) == 1.0f);
        REQUIRE(matrix(0, 2) == 2.0f);
        REQUIRE(matrix(1, 0) == 3.0f);
        REQUIRE(matrix(1, 1) == 4.0f);
        REQUIRE(matrix(1, 2) == 5.0f);
        REQUIRE(matrix(2, 0) == 6.0f);
        REQUIRE(matrix(2, 1) == 7.0f);
        REQUIRE(matrix(2, 2) == 8.0f);
    }
    SUBCASE("createIdentity")
    {
        Matrix3x3 matrix = Matrix3x3::createIdentity();
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 1.0f);
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == 1.0f);
    }
    SUBCASE("multiply scalar")
    {
        REQUIRE(
            Matrix3x3{
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f} ==
            Matrix3x3::createIdentity() * 0.0f);

        REQUIRE(
            Matrix3x3::createIdentity() ==
            Matrix3x3::createIdentity() * 1.0f);

        REQUIRE(
            Matrix3x3{
                4.0f, 0.0f, 0.0f,
                0.0f, 4.0f, 0.0f,
                0.0f, 0.0f, 4.0f} ==
            Matrix3x3::createIdentity() * 4.0f);
    }
    SUBCASE("multiply matrix")
    {
        REQUIRE(
            Matrix3x3{
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f} ==
            multiply(
                Matrix3x3::createIdentity(),
                Matrix3x3::createIdentity()));

        REQUIRE(
            Matrix3x3{
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f} ==
            multiply(
                Matrix3x3::createIdentity(),
                Matrix3x3{
                    0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f}));

        REQUIRE(
            Matrix3x3{
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f} ==
            multiply(
                Matrix3x3{
                    0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f},
                Matrix3x3::createIdentity()));
    }
    SUBCASE("data")
    {
        Matrix3x3 matrix{
            0.0f, 1.0f, 2.0f,
            3.0f, 4.0f, 5.0f,
            6.0f, 7.0f, 8.0f};
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        REQUIRE(0.0f == *(matrix.data() + 0));
        REQUIRE(1.0f == *(matrix.data() + 1));
        REQUIRE(2.0f == *(matrix.data() + 2));
        REQUIRE(3.0f == *(matrix.data() + 3));
        REQUIRE(4.0f == *(matrix.data() + 4));
        REQUIRE(5.0f == *(matrix.data() + 5));
        REQUIRE(6.0f == *(matrix.data() + 6));
        REQUIRE(7.0f == *(matrix.data() + 7));
        REQUIRE(8.0f == *(matrix.data() + 8));
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }
    SUBCASE("determinant")
    {
        using pomdog::math::determinant;
        REQUIRE(
            approx(-78.0f) ==
            determinant(Matrix3x3{
                3.0f, 4.0f, 5.0f,
                2.0f, 7.0f, -1.0f,
                4.0f, 1.0f, 5.0f}));

        REQUIRE(
            approx(234.0f) ==
            determinant(Matrix3x3{
                3.0f, 4.0f, 5.0f,
                2.0f, 7.0f, -1.0f,
                -4.0f, 1.0f, 5.0f}));

        REQUIRE(
            approx(1.0f) ==
            determinant(Matrix3x3::createIdentity()));
    }
    SUBCASE("adjoint")
    {
        using pomdog::math::adjoint;
        REQUIRE(
            Matrix3x3{
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f} ==
            adjoint(Matrix3x3::createIdentity()));

        REQUIRE(
            Matrix3x3{
                36.0f, -15.0f, -39.0f,
                -14.0f, -5.0f, 13.0f,
                -26.0f, 13.0f, 13.0f} ==
            adjoint(Matrix3x3{
                3.0f, 4.0f, 5.0f,
                2.0f, 7.0f, -1.0f,
                4.0f, 1.0f, 5.0f}));
    }
    SUBCASE("invert")
    {
        auto matrix = Matrix3x3{
            3.0f, 4.0f, 5.0f,
            2.0f, 7.0f, -1.0f,
            4.0f, 1.0f, 5.0f};

        auto inverted = pomdog::math::invert(matrix);
        auto result = inverted * matrix;

        REQUIRE(result(0, 0) == approx(1.0f));
        REQUIRE(result(0, 1) == approx(0.0f));
        REQUIRE(result(0, 2) == approx(0.0f));
        REQUIRE(result(1, 0) == approx(0.0f));
        REQUIRE(result(1, 1) == approx(1.0f));
        REQUIRE(result(1, 2) == approx(0.0f));
        REQUIRE(result(2, 0) == approx(0.0f));
        REQUIRE(result(2, 1) == approx(0.0f));
        REQUIRE(result(2, 2) == approx(1.0f));
    }
    SUBCASE("invert with exact inverses")
    {
        // NOTE: The elements and cofactors of these matrices are small integers
        // and powers of two, so every product is exact and the inverse matches
        // the expected matrix bit for bit (0 and -0 compare equal).
        using pomdog::math::invert;
        REQUIRE(invert(Matrix3x3::createIdentity()) == Matrix3x3::createIdentity());
        REQUIRE(
            invert(Matrix3x3::createScale(Vector3{2.0f, 4.0f, 8.0f})) ==
            Matrix3x3::createScale(Vector3{0.5f, 0.25f, 0.125f}));

        const auto lower = Matrix3x3{
            1.0f, 0.0f, 0.0f,
            2.0f, 1.0f, 0.0f,
            0.0f, 3.0f, 1.0f};
        const auto inverseLower = Matrix3x3{
            1.0f, 0.0f, 0.0f,
            -2.0f, 1.0f, 0.0f,
            6.0f, -3.0f, 1.0f};
        const auto upper = Matrix3x3{
            1.0f, 4.0f, 0.0f,
            0.0f, 1.0f, 5.0f,
            0.0f, 0.0f, 1.0f};
        const auto inverseUpper = Matrix3x3{
            1.0f, -4.0f, 20.0f,
            0.0f, 1.0f, -5.0f,
            0.0f, 0.0f, 1.0f};
        REQUIRE(invert(lower) == inverseLower);
        REQUIRE(invert(upper) == inverseUpper);
        REQUIRE(invert(lower * upper) == inverseUpper * inverseLower);
        REQUIRE(invert(upper * lower) == inverseLower * inverseUpper);
    }
    SUBCASE("createTranslation")
    {
        auto matrix = Matrix3x3::createTranslation(Vector2{3.0f, 4.0f});
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 1.0f);
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(2, 0) == 3.0f);
        REQUIRE(matrix(2, 1) == 4.0f);
        REQUIRE(matrix(2, 2) == 1.0f);
    }
    SUBCASE("createScale")
    {
        auto matrix = Matrix3x3::createScale(Vector3{3.0f, 4.0f, 5.0f});
        REQUIRE(matrix(0, 0) == 3.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 4.0f);
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == 5.0f);

        matrix = Matrix3x3::createScale(7.0f);
        REQUIRE(matrix(0, 0) == 7.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 7.0f);
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == 7.0f);
    }
    SUBCASE("createRotationX")
    {
        auto matrix = Matrix3x3::createRotationX(Radian32{3.0f});
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == approx(std::cos(3.0f)));
        REQUIRE(matrix(1, 2) == approx(std::sin(3.0f)));
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == approx(-std::sin(3.0f)));
        REQUIRE(matrix(2, 2) == approx(std::cos(3.0f)));
    }
    SUBCASE("createRotationY")
    {
        auto matrix = Matrix3x3::createRotationY(Radian32{3.0f});
        REQUIRE(matrix(0, 0) == approx(std::cos(3.0f)));
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == approx(-std::sin(3.0f)));
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 1.0f);
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(2, 0) == approx(std::sin(3.0f)));
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == approx(std::cos(3.0f)));
    }
    SUBCASE("createRotationZ")
    {
        auto matrix = Matrix3x3::createRotationZ(Radian32{3.0f});
        REQUIRE(matrix(0, 0) == approx(std::cos(3.0f)));
        REQUIRE(matrix(0, 1) == approx(std::sin(3.0f)));
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(1, 0) == approx(-std::sin(3.0f)));
        REQUIRE(matrix(1, 1) == approx(std::cos(3.0f)));
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == 1.0f);
    }
    SUBCASE("createRotationX, createRotationY, and createRotationZ with turns")
    {
        // NOTE: At quarter turns, the elements are exact, so they equal the
        // radian results rounded to -1, 0, or 1. Other angles match the radian
        // results closely.
        const auto check = [](const Matrix3x3& actual, const Matrix3x3& expected, bool exact) {
            for (i32 row = 0; row < 3; ++row) {
                for (i32 column = 0; column < 3; ++column) {
                    CAPTURE(row);
                    CAPTURE(column);
                    if (exact) {
                        REQUIRE_EQ(actual(row, column), std::round(expected(row, column)));
                    }
                    else {
                        REQUIRE(actual(row, column) == doctest::Approx(expected(row, column)).epsilon(1e-5));
                    }
                }
            }
        };
        constexpr std::array<f32, 9> angles = {{-0.25f, 0.0f, 0.25f, 0.5f, 0.75f, 1.0f, -0.375f, 0.1f, 0.6f}};
        for (const f32 turns : angles) {
            CAPTURE(turns);
            const bool isQuarterTurn = (turns * 4.0f) == std::floor(turns * 4.0f);
            const auto radians = pomdog::math::toRadian(Turn32{turns});
            check(Matrix3x3::createRotationX(Turn32{turns}), Matrix3x3::createRotationX(radians), isQuarterTurn);
            check(Matrix3x3::createRotationY(Turn32{turns}), Matrix3x3::createRotationY(radians), isQuarterTurn);
            check(Matrix3x3::createRotationZ(Turn32{turns}), Matrix3x3::createRotationZ(radians), isQuarterTurn);
        }
    }
    SUBCASE("createFromAxisAngle with turns")
    {
        // NOTE: Quarter turns around a coordinate axis give the same exact
        // elements as createRotationX, createRotationY, and createRotationZ.
        const auto requireSame = [](const Matrix3x3& actual, const Matrix3x3& expected) {
            for (i32 row = 0; row < 3; ++row) {
                for (i32 column = 0; column < 3; ++column) {
                    CAPTURE(row);
                    CAPTURE(column);
                    REQUIRE_EQ(actual(row, column), expected(row, column));
                }
            }
        };
        constexpr std::array<f32, 6> quarterTurns = {{-0.25f, 0.0f, 0.25f, 0.5f, 0.75f, 1.0f}};
        for (const f32 turns : quarterTurns) {
            CAPTURE(turns);
            requireSame(Matrix3x3::createFromAxisAngle(Vector3::createUnitX(), Turn32{turns}), Matrix3x3::createRotationX(Turn32{turns}));
            requireSame(Matrix3x3::createFromAxisAngle(Vector3::createUnitY(), Turn32{turns}), Matrix3x3::createRotationY(Turn32{turns}));
            requireSame(Matrix3x3::createFromAxisAngle(Vector3::createUnitZ(), Turn32{turns}), Matrix3x3::createRotationZ(Turn32{turns}));
        }

        // NOTE: Other axes and angles match the radian overload.
        const auto axis = Vector3{0.6f, 0.0f, 0.8f};
        constexpr std::array<f32, 4> angles = {{-0.375f, 0.1f, 0.6f, 2.125f}};
        for (const f32 turns : angles) {
            CAPTURE(turns);
            const auto actual = Matrix3x3::createFromAxisAngle(axis, Turn32{turns});
            const auto expected = Matrix3x3::createFromAxisAngle(axis, pomdog::math::toRadian(Turn32{turns}));
            for (i32 row = 0; row < 3; ++row) {
                for (i32 column = 0; column < 3; ++column) {
                    CAPTURE(row);
                    CAPTURE(column);
                    REQUIRE(actual(row, column) == doctest::Approx(expected(row, column)).epsilon(1e-5));
                }
            }
        }
    }
}

TEST_CASE("Matrix3x3::createFromQuaternion")
{
    const auto requireElements = [](const Matrix3x3& actual, const std::array<f32, 9>& expected) {
        for (i32 row = 0; row < 3; ++row) {
            for (i32 column = 0; column < 3; ++column) {
                CAPTURE(row);
                CAPTURE(column);
                REQUIRE_EQ(actual(row, column), expected[static_cast<std::size_t>(row * 3 + column)]);
            }
        }
    };
    SUBCASE("identity")
    {
        requireElements(
            Matrix3x3::createFromQuaternion(Quaternion{0.0f, 0.0f, 0.0f, 1.0f}),
            {{1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 1.0f}});
    }
    SUBCASE("every element")
    {
        // NOTE: An unnormalized quaternion whose elements are distinct small
        // integers, so each element is exact and lands in one place only.
        requireElements(
            Matrix3x3::createFromQuaternion(Quaternion{3.0f, -4.0f, 5.0f, 6.0f}),
            {{-81.0f, 36.0f, 78.0f,
                -84.0f, -67.0f, -4.0f,
                -18.0f, -76.0f, -49.0f}});
    }
    SUBCASE("rotation around an axis")
    {
        const auto axis = Vector3{0.6f, 0.0f, 0.8f};
        constexpr std::array<f32, 4> angles = {{-0.375f, 0.1f, 0.6f, 2.125f}};
        for (const f32 turns : angles) {
            CAPTURE(turns);
            const auto actual = Matrix3x3::createFromQuaternion(Quaternion::createFromAxisAngle(axis, Turn32{turns}));
            const auto expected = Matrix3x3::createFromAxisAngle(axis, Turn32{turns});
            for (i32 row = 0; row < 3; ++row) {
                for (i32 column = 0; column < 3; ++column) {
                    CAPTURE(row);
                    CAPTURE(column);
                    REQUIRE(actual(row, column) == doctest::Approx(expected(row, column)).epsilon(1e-5));
                }
            }
        }
    }
}
