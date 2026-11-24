// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/quaternion.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/turn.h"
#include "pomdog/math/vector3.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::f32;
using pomdog::i32;
using pomdog::Matrix4x4;
using pomdog::Quaternion;
using pomdog::Radian32;
using pomdog::Turn32;
using pomdog::Vector3;
using pomdog::math::multiply;
using pomdog::math::toRadian;

namespace {
auto approx(float x) -> auto
{
    return doctest::Approx(static_cast<double>(x)).epsilon(0.000001);
}
} // namespace

TEST_CASE("Matrix4x4")
{
    SUBCASE("constructor")
    {
        Matrix4x4 matrix{
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f};
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 1.0f);
        REQUIRE(matrix(0, 2) == 1.0f);
        REQUIRE(matrix(0, 3) == 1.0f);
        REQUIRE(matrix(1, 0) == 1.0f);
        REQUIRE(matrix(1, 1) == 1.0f);
        REQUIRE(matrix(1, 2) == 1.0f);
        REQUIRE(matrix(1, 3) == 1.0f);
        REQUIRE(matrix(2, 0) == 1.0f);
        REQUIRE(matrix(2, 1) == 1.0f);
        REQUIRE(matrix(2, 2) == 1.0f);
        REQUIRE(matrix(2, 3) == 1.0f);
        REQUIRE(matrix(3, 0) == 1.0f);
        REQUIRE(matrix(3, 1) == 1.0f);
        REQUIRE(matrix(3, 2) == 1.0f);
        REQUIRE(matrix(3, 3) == 1.0f);

        matrix = Matrix4x4{
            0.0f, 1.0f, 2.0f, 3.0f,
            4.0f, 5.0f, 6.0f, 7.0f,
            8.0f, 9.0f, 10.0f, 11.0f,
            12.0f, 13.0f, 14.0f, 15.0f};
        REQUIRE(matrix(0, 0) == 0.0f);
        REQUIRE(matrix(0, 1) == 1.0f);
        REQUIRE(matrix(0, 2) == 2.0f);
        REQUIRE(matrix(0, 3) == 3.0f);
        REQUIRE(matrix(1, 0) == 4.0f);
        REQUIRE(matrix(1, 1) == 5.0f);
        REQUIRE(matrix(1, 2) == 6.0f);
        REQUIRE(matrix(1, 3) == 7.0f);
        REQUIRE(matrix(2, 0) == 8.0f);
        REQUIRE(matrix(2, 1) == 9.0f);
        REQUIRE(matrix(2, 2) == 10.0f);
        REQUIRE(matrix(2, 3) == 11.0f);
        REQUIRE(matrix(3, 0) == 12.0f);
        REQUIRE(matrix(3, 1) == 13.0f);
        REQUIRE(matrix(3, 2) == 14.0f);
        REQUIRE(matrix(3, 3) == 15.0f);
    }
    SUBCASE("createIdentity")
    {
        auto matrix = Matrix4x4::createIdentity();
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(0, 3) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 1.0f);
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(1, 3) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == 1.0f);
        REQUIRE(matrix(2, 3) == 0.0f);
        REQUIRE(matrix(3, 0) == 0.0f);
        REQUIRE(matrix(3, 1) == 0.0f);
        REQUIRE(matrix(3, 2) == 0.0f);
        REQUIRE(matrix(3, 3) == 1.0f);
    }
    SUBCASE("multiply scalar")
    {
        REQUIRE(
            Matrix4x4{
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f} ==
            Matrix4x4::createIdentity() * 0.0f);

        REQUIRE(
            Matrix4x4::createIdentity() ==
            Matrix4x4::createIdentity() * 1.0f);

        REQUIRE(
            Matrix4x4{
                4.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 4.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 4.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 4.0f} ==
            Matrix4x4::createIdentity() * 4.0f);
    }
    SUBCASE("multiply matrix")
    {
        REQUIRE(
            Matrix4x4::createIdentity() ==
            multiply(
                Matrix4x4::createIdentity(),
                Matrix4x4::createIdentity()));

        REQUIRE(
            Matrix4x4{
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f} ==
            multiply(
                Matrix4x4::createIdentity(),
                Matrix4x4{
                    0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f}));

        REQUIRE(
            Matrix4x4{
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f} ==
            multiply(
                Matrix4x4{
                    0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f},
                Matrix4x4::createIdentity()));
    }
    SUBCASE("data")
    {
        Matrix4x4 matrix{
            0.0f, 1.0f, 2.0f, 3.0f,
            4.0f, 5.0f, 6.0f, 7.0f,
            8.0f, 9.0f, 10.0f, 11.0f,
            12.0f, 13.0f, 14.0f, 15.0f};
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
        REQUIRE(9.0f == *(matrix.data() + 9));
        REQUIRE(10.0f == *(matrix.data() + 10));
        REQUIRE(11.0f == *(matrix.data() + 11));
        REQUIRE(12.0f == *(matrix.data() + 12));
        REQUIRE(13.0f == *(matrix.data() + 13));
        REQUIRE(14.0f == *(matrix.data() + 14));
        REQUIRE(15.0f == *(matrix.data() + 15));
        POMDOG_CLANG_UNSAFE_BUFFER_END
    }
    SUBCASE("transpose")
    {
        using pomdog::math::transpose;
        REQUIRE(
            Matrix4x4::createIdentity() ==
            transpose(Matrix4x4::createIdentity()));

        REQUIRE(Matrix4x4{
                    1.0f, 5.0f, 9.0f, 13.0f,
                    2.0f, 6.0f, 10.0f, 14.0f,
                    3.0f, 7.0f, 11.0f, 15.0f,
                    4.0f, 8.0f, 12.0f, 16.0f} ==
                transpose(Matrix4x4{
                    1.0f, 2.0f, 3.0f, 4.0f,
                    5.0f, 6.0f, 7.0f, 8.0f,
                    9.0f, 10.0f, 11.0f, 12.0f,
                    13.0f, 14.0f, 15.0f, 16.0f}));
    }
    SUBCASE("createTranslation")
    {
        auto matrix = Matrix4x4::createTranslation(Vector3{3.0f, 4.0f, 5.0f});
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(0, 3) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 1.0f);
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(1, 3) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == 1.0f);
        REQUIRE(matrix(2, 3) == 0.0f);
        REQUIRE(matrix(3, 0) == 3.0f);
        REQUIRE(matrix(3, 1) == 4.0f);
        REQUIRE(matrix(3, 2) == 5.0f);
        REQUIRE(matrix(3, 3) == 1.0f);
    }
    SUBCASE("createScale")
    {
        auto matrix = Matrix4x4::createScale(Vector3{3.0f, 4.0f, 5.0f});
        REQUIRE(matrix(0, 0) == 3.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(0, 3) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 4.0f);
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(1, 3) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == 5.0f);
        REQUIRE(matrix(2, 3) == 0.0f);
        REQUIRE(matrix(3, 0) == 0.0f);
        REQUIRE(matrix(3, 1) == 0.0f);
        REQUIRE(matrix(3, 2) == 0.0f);
        REQUIRE(matrix(3, 3) == 1.0f);

        matrix = Matrix4x4::createScale(7.0f);
        REQUIRE(matrix(0, 0) == 7.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(0, 3) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 7.0f);
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(1, 3) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == 7.0f);
        REQUIRE(matrix(2, 3) == 0.0f);
        REQUIRE(matrix(3, 0) == 0.0f);
        REQUIRE(matrix(3, 1) == 0.0f);
        REQUIRE(matrix(3, 2) == 0.0f);
        REQUIRE(matrix(3, 3) == 1.0f);
    }
    SUBCASE("createRotationX")
    {
        auto matrix = Matrix4x4::createRotationX(Radian32{3.0f});
        REQUIRE(matrix(0, 0) == 1.0f);
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(0, 3) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == approx(std::cos(3.0f)));
        REQUIRE(matrix(1, 2) == approx(std::sin(3.0f)));
        REQUIRE(matrix(1, 3) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == approx(-std::sin(3.0f)));
        REQUIRE(matrix(2, 2) == approx(std::cos(3.0f)));
        REQUIRE(matrix(2, 3) == 0.0f);
        REQUIRE(matrix(3, 0) == 0.0f);
        REQUIRE(matrix(3, 1) == 0.0f);
        REQUIRE(matrix(3, 2) == 0.0f);
        REQUIRE(matrix(3, 3) == 1.0f);
    }
    SUBCASE("createRotationY")
    {
        auto matrix = Matrix4x4::createRotationY(Radian32{3.0f});
        REQUIRE(matrix(0, 0) == approx(std::cos(3.0f)));
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == approx(-std::sin(3.0f)));
        REQUIRE(matrix(0, 3) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == 1.0f);
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(1, 3) == 0.0f);
        REQUIRE(matrix(2, 0) == approx(std::sin(3.0f)));
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == approx(std::cos(3.0f)));
        REQUIRE(matrix(2, 3) == 0.0f);
        REQUIRE(matrix(3, 0) == 0.0f);
        REQUIRE(matrix(3, 1) == 0.0f);
        REQUIRE(matrix(3, 2) == 0.0f);
        REQUIRE(matrix(3, 3) == 1.0f);
    }
    SUBCASE("createRotationZ")
    {
        auto matrix = Matrix4x4::createRotationZ(Radian32{3.0f});
        REQUIRE(matrix(0, 0) == approx(std::cos(3.0f)));
        REQUIRE(matrix(0, 1) == approx(std::sin(3.0f)));
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(0, 3) == 0.0f);
        REQUIRE(matrix(1, 0) == approx(-std::sin(3.0f)));
        REQUIRE(matrix(1, 1) == approx(std::cos(3.0f)));
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(1, 3) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == 1.0f);
        REQUIRE(matrix(2, 3) == 0.0f);
        REQUIRE(matrix(3, 0) == 0.0f);
        REQUIRE(matrix(3, 1) == 0.0f);
        REQUIRE(matrix(3, 2) == 0.0f);
        REQUIRE(matrix(3, 3) == 1.0f);
    }
    SUBCASE("createRotationX, createRotationY, and createRotationZ with turns")
    {
        // NOTE: At quarter turns, the elements are exact, so they equal the
        // radian results rounded to -1, 0, or 1. Other angles match the radian
        // results closely.
        const auto check = [](const Matrix4x4& actual, const Matrix4x4& expected, bool exact) {
            for (i32 row = 0; row < 4; ++row) {
                for (i32 column = 0; column < 4; ++column) {
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
            check(Matrix4x4::createRotationX(Turn32{turns}), Matrix4x4::createRotationX(radians), isQuarterTurn);
            check(Matrix4x4::createRotationY(Turn32{turns}), Matrix4x4::createRotationY(radians), isQuarterTurn);
            check(Matrix4x4::createRotationZ(Turn32{turns}), Matrix4x4::createRotationZ(radians), isQuarterTurn);
        }
    }
    SUBCASE("createFromAxisAngle with turns")
    {
        // NOTE: Quarter turns around a coordinate axis give the same exact
        // elements as createRotationX, createRotationY, and createRotationZ.
        const auto requireSame = [](const Matrix4x4& actual, const Matrix4x4& expected) {
            for (i32 row = 0; row < 4; ++row) {
                for (i32 column = 0; column < 4; ++column) {
                    CAPTURE(row);
                    CAPTURE(column);
                    REQUIRE_EQ(actual(row, column), expected(row, column));
                }
            }
        };
        constexpr std::array<f32, 6> quarterTurns = {{-0.25f, 0.0f, 0.25f, 0.5f, 0.75f, 1.0f}};
        for (const f32 turns : quarterTurns) {
            CAPTURE(turns);
            requireSame(Matrix4x4::createFromAxisAngle(Vector3::createUnitX(), Turn32{turns}), Matrix4x4::createRotationX(Turn32{turns}));
            requireSame(Matrix4x4::createFromAxisAngle(Vector3::createUnitY(), Turn32{turns}), Matrix4x4::createRotationY(Turn32{turns}));
            requireSame(Matrix4x4::createFromAxisAngle(Vector3::createUnitZ(), Turn32{turns}), Matrix4x4::createRotationZ(Turn32{turns}));
        }

        // NOTE: Other axes and angles match the radian overload.
        const auto axis = Vector3{0.6f, 0.0f, 0.8f};
        constexpr std::array<f32, 4> angles = {{-0.375f, 0.1f, 0.6f, 2.125f}};
        for (const f32 turns : angles) {
            CAPTURE(turns);
            const auto actual = Matrix4x4::createFromAxisAngle(axis, Turn32{turns});
            const auto expected = Matrix4x4::createFromAxisAngle(axis, pomdog::math::toRadian(Turn32{turns}));
            for (i32 row = 0; row < 4; ++row) {
                for (i32 column = 0; column < 4; ++column) {
                    CAPTURE(row);
                    CAPTURE(column);
                    REQUIRE(actual(row, column) == doctest::Approx(expected(row, column)).epsilon(1e-5));
                }
            }
        }
    }
    SUBCASE("createPerspectiveFieldOfViewLH")
    {
        const auto matrix = Matrix4x4::createPerspectiveFieldOfViewLH(toRadian(45.0f), 800.0f / 480.0f, 0.01f, 1000.0f);

        constexpr auto piOver180 = 3.14159265358979323846f / 180.0f;
        const auto h = 1.0f / std::tan(45.0f * piOver180 * 0.5f);
        const auto w = h / (800.0f / 480.0f);
        const auto a = 1000.0f / (1000.0f - 0.01f);
        const auto b = (-0.01f * 1000.0f) / (1000.0f - 0.01f);

        REQUIRE(matrix(0, 0) == approx(w));
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(0, 3) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == approx(h));
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(1, 3) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == approx(a));
        REQUIRE(matrix(2, 3) == 1.0f);
        REQUIRE(matrix(3, 0) == 0.0f);
        REQUIRE(matrix(3, 1) == 0.0f);
        REQUIRE(matrix(3, 2) == approx(b));
        REQUIRE(matrix(3, 3) == 0.0f);
    }
    SUBCASE("createPerspectiveFieldOfViewRH")
    {
        const auto matrix = Matrix4x4::createPerspectiveFieldOfViewRH(toRadian(45.0f), 800.0f / 480.0f, 0.01f, 1000.0f);

        constexpr auto piOver180 = 3.14159265358979323846f / 180.0f;
        const auto h = 1.0f / std::tan(45.0f * piOver180 * 0.5f);
        const auto w = h / (800.0f / 480.0f);
        const auto a = 1000.0f / (0.01f - 1000.0f);
        const auto b = (0.01f * 1000.0f) / (0.01f - 1000.0f);

        REQUIRE(matrix(0, 0) == approx(w));
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(0, 3) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == approx(h));
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(1, 3) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == approx(a));
        REQUIRE(matrix(2, 3) == -1.0f);
        REQUIRE(matrix(3, 0) == 0.0f);
        REQUIRE(matrix(3, 1) == 0.0f);
        REQUIRE(matrix(3, 2) == approx(b));
        REQUIRE(matrix(3, 3) == 0.0f);
    }
    SUBCASE("createOrthographicLH")
    {
        auto matrix = Matrix4x4::createOrthographicLH(800.0f, 480.0f, 0.5f, 1000.0f);

        REQUIRE(matrix(0, 0) == approx(2.0f / 800.0f));
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(0, 3) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == approx(2.0f / 480.0f));
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(1, 3) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == approx(1.0f / (1000.0f - 0.5f)));
        REQUIRE(matrix(2, 3) == 0.0f);
        REQUIRE(matrix(3, 0) == 0.0f);
        REQUIRE(matrix(3, 1) == 0.0f);
        REQUIRE(matrix(3, 2) == approx(0.5f / (0.5f - 1000.0f)));
        REQUIRE(matrix(3, 3) == 1.0f);
    }
    SUBCASE("createOrthographicRH")
    {
        auto matrix = Matrix4x4::createOrthographicRH(800.0f, 480.0f, 0.5f, 1000.0f);

        REQUIRE(matrix(0, 0) == approx(2.0f / 800.0f));
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(0, 3) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == approx(2.0f / 480.0f));
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(1, 3) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == approx(1.0f / (0.5f - 1000.0f)));
        REQUIRE(matrix(2, 3) == 0.0f);
        REQUIRE(matrix(3, 0) == 0.0f);
        REQUIRE(matrix(3, 1) == 0.0f);
        REQUIRE(matrix(3, 2) == approx(0.5f / (0.5f - 1000.0f)));
        REQUIRE(matrix(3, 3) == 1.0f);
    }
    SUBCASE("createOrthographicOffCenterLH")
    {
        auto matrix = Matrix4x4::createOrthographicOffCenterLH(0.0f, 800.0f, 0.0f, 480.0f, 0.5f, 1000.0f);

        REQUIRE(matrix(0, 0) == approx(2.0f / 800.0f));
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(0, 3) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == approx(2.0f / 480.0f));
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(1, 3) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == approx(1.0f / (1000.0f - 0.5f)));
        REQUIRE(matrix(2, 3) == 0.0f);
        REQUIRE(matrix(3, 0) == approx((0.0f + 800.0f) / (0.0f - 800.0f)));
        REQUIRE(matrix(3, 1) == approx((0.0f + 480.0f) / (0.0f - 480.0f)));
        REQUIRE(matrix(3, 2) == approx(0.5f / (0.5f - 1000.0f)));
        REQUIRE(matrix(3, 3) == 1.0f);
    }
    SUBCASE("createOrthographicOffCenterRH")
    {
        auto matrix = Matrix4x4::createOrthographicOffCenterRH(0.0f, 800.0f, 0.0f, 480.0f, 0.5f, 1000.0f);

        REQUIRE(matrix(0, 0) == approx(2.0f / 800.0f));
        REQUIRE(matrix(0, 1) == 0.0f);
        REQUIRE(matrix(0, 2) == 0.0f);
        REQUIRE(matrix(0, 3) == 0.0f);
        REQUIRE(matrix(1, 0) == 0.0f);
        REQUIRE(matrix(1, 1) == approx(2.0f / 480.0f));
        REQUIRE(matrix(1, 2) == 0.0f);
        REQUIRE(matrix(1, 3) == 0.0f);
        REQUIRE(matrix(2, 0) == 0.0f);
        REQUIRE(matrix(2, 1) == 0.0f);
        REQUIRE(matrix(2, 2) == approx(1.0f / (0.5f - 1000.0f)));
        REQUIRE(matrix(2, 3) == 0.0f);
        REQUIRE(matrix(3, 0) == approx((0.0f + 800.0f) / (0.0f - 800.0f)));
        REQUIRE(matrix(3, 1) == approx((0.0f + 480.0f) / (0.0f - 480.0f)));
        REQUIRE(matrix(3, 2) == approx(0.5f / (0.5f - 1000.0f)));
        REQUIRE(matrix(3, 3) == 1.0f);
    }
}

TEST_CASE("Matrix4x4 determinant, adjoint, and invert")
{
    SUBCASE("determinant")
    {
        // NOTE: The elements and minors of these matrices are small integers,
        // so every product is exact and the determinant matches bit for bit.
        using pomdog::math::determinant;
        REQUIRE_EQ(
            determinant(Matrix4x4{
                3.0f, 4.0f, 9.0f, 2.0f,
                1.0f, 3.0f, 3.0f, 7.0f,
                5.0f, 2.0f, -4.0f, 1.0f,
                9.0f, 4.0f, 3.0f, 5.0f}),
            -945.0f);
        REQUIRE_EQ(determinant(Matrix4x4::createIdentity()), 1.0f);
        REQUIRE_EQ(determinant(Matrix4x4::createScale(Vector3{2.0f, 4.0f, 8.0f})), 64.0f);
        REQUIRE_EQ(determinant(Matrix4x4::createTranslation(Vector3{3.0f, -5.0f, 7.0f})), 1.0f);
        REQUIRE_EQ(determinant(Matrix4x4::createRotationZ(Turn32{0.25f})), 1.0f);

        // NOTE: Swapping two rows negates the determinant.
        REQUIRE_EQ(
            determinant(Matrix4x4{
                0.0f, 1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f}),
            -1.0f);

        // NOTE: Row 1 is twice row 0.
        REQUIRE_EQ(
            determinant(Matrix4x4{
                1.0f, 2.0f, 3.0f, 4.0f,
                2.0f, 4.0f, 6.0f, 8.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f}),
            0.0f);
    }
    SUBCASE("equal rows with inexact products have zero minors")
    {
        const Matrix4x4 singular{
            10, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 409.6f, 409.6f,
            0, 0, 409.6f, 409.6f};
        REQUIRE_EQ(pomdog::math::determinant(singular), 0.0f);
    }
    SUBCASE("equal products cancel in the cofactor subtraction and addition")
    {
        // NOTE: x*x rounds down by one in f32. The column order determines
        // which two terms of the cofactor must cancel.
        constexpr f32 x = 4097.0f;
        using pomdog::math::determinant;
        CHECK_EQ(
            determinant(Matrix4x4{
                0, 0, 1, 0,
                x, x, 0, 0,
                x, x, 0, 0,
                0, 0, 0, 1}),
            0.0f);
        CHECK_EQ(
            determinant(Matrix4x4{
                0, 1, 0, 0,
                x, 0, 0, x,
                x, 0, 0, x,
                0, 0, 1, 0}),
            0.0f);
    }
    SUBCASE("adjoint")
    {
        // NOTE: The elements and cofactors of these matrices are small integers,
        // so every product is exact. For a non-singular A, adj(A) is the only
        // matrix with A * adj(A) = adj(A) * A = det(A) * I.
        using pomdog::math::adjoint;
        using pomdog::math::determinant;
        const auto requireSame = [](const Matrix4x4& actual, const Matrix4x4& expected) {
            for (i32 row = 0; row < 4; ++row) {
                for (i32 column = 0; column < 4; ++column) {
                    CAPTURE(row);
                    CAPTURE(column);
                    REQUIRE_EQ(actual(row, column), expected(row, column));
                }
            }
        };

        requireSame(adjoint(Matrix4x4::createIdentity()), Matrix4x4::createIdentity());
        requireSame(
            adjoint(Matrix4x4::createScale(Vector3{2.0f, 4.0f, 8.0f})),
            Matrix4x4{
                32.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 16.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 8.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 64.0f});

        const auto matrix = Matrix4x4{
            3.0f, 4.0f, 9.0f, 2.0f,
            1.0f, 3.0f, 3.0f, 7.0f,
            5.0f, 2.0f, -4.0f, 1.0f,
            9.0f, 4.0f, 3.0f, 5.0f};
        const auto adjugate = adjoint(matrix);
        const auto scaledIdentity = Matrix4x4::createIdentity() * determinant(matrix);
        requireSame(matrix * adjugate, scaledIdentity);
        requireSame(adjugate * matrix, scaledIdentity);

        // NOTE: Row 1 is twice row 0, so this matrix has rank 3 and its
        // adjugate has rank 1.
        requireSame(
            adjoint(Matrix4x4{
                1.0f, 2.0f, 3.0f, 4.0f,
                2.0f, 4.0f, 6.0f, 8.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f}),
            Matrix4x4{
                8.0f, -4.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                -2.0f, 1.0f, 0.0f, 0.0f});
    }
    SUBCASE("invert")
    {
        auto matrix = Matrix4x4{
            3.0f, 4.0f, 9.0f, 2.0f,
            1.0f, 3.0f, 3.0f, 7.0f,
            5.0f, 2.0f, -4.0f, 1.0f,
            9.0f, 4.0f, 3.0f, 5.0f};

        auto inverted = pomdog::math::invert(matrix);
        auto result = inverted * matrix;

        REQUIRE(result(0, 0) == approx(1.0f));
        REQUIRE(result(0, 1) == approx(0.0f));
        REQUIRE(result(0, 2) == approx(0.0f));
        REQUIRE(result(0, 3) == approx(0.0f));
        REQUIRE(result(1, 0) == approx(0.0f));
        REQUIRE(result(1, 1) == approx(1.0f));
        REQUIRE(result(1, 2) == approx(0.0f));
        REQUIRE(result(1, 3) == approx(0.0f));
        REQUIRE(result(2, 0) == approx(0.0f));
        REQUIRE(result(2, 1) == approx(0.0f));
        REQUIRE(result(2, 2) == approx(1.0f));
        REQUIRE(result(2, 3) == approx(0.0f));
        REQUIRE(result(3, 0) == approx(0.0f));
        REQUIRE(result(3, 1) == approx(0.0f));
        REQUIRE(result(3, 2) == approx(0.0f));
        REQUIRE(result(3, 3) == approx(1.0f));
    }
    SUBCASE("invert with exact inverses")
    {
        // NOTE: The elements and cofactors of these matrices are small integers
        // and powers of two, so every product is exact and the inverse matches
        // the expected matrix bit for bit (0 and -0 compare equal).
        using pomdog::math::invert;
        const auto requireSame = [](const Matrix4x4& actual, const Matrix4x4& expected) {
            for (i32 row = 0; row < 4; ++row) {
                for (i32 column = 0; column < 4; ++column) {
                    CAPTURE(row);
                    CAPTURE(column);
                    REQUIRE_EQ(actual(row, column), expected(row, column));
                }
            }
        };
        const auto scale = Matrix4x4::createScale(Vector3{2.0f, 4.0f, 8.0f});
        const auto inverseScale = Matrix4x4::createScale(Vector3{0.5f, 0.25f, 0.125f});
        const auto translation = Matrix4x4::createTranslation(Vector3{3.0f, -5.0f, 7.0f});
        const auto inverseTranslation = Matrix4x4::createTranslation(Vector3{-3.0f, 5.0f, -7.0f});

        requireSame(invert(Matrix4x4::createIdentity()), Matrix4x4::createIdentity());
        requireSame(invert(scale), inverseScale);
        requireSame(invert(translation), inverseTranslation);
        requireSame(invert(Matrix4x4::createRotationX(Turn32{0.25f})), Matrix4x4::createRotationX(Turn32{-0.25f}));
        requireSame(invert(Matrix4x4::createRotationY(Turn32{0.25f})), Matrix4x4::createRotationY(Turn32{-0.25f}));
        requireSame(invert(Matrix4x4::createRotationZ(Turn32{0.25f})), Matrix4x4::createRotationZ(Turn32{-0.25f}));
        requireSame(
            invert(scale * Matrix4x4::createRotationX(Turn32{0.25f}) * translation),
            inverseTranslation * Matrix4x4::createRotationX(Turn32{-0.25f}) * inverseScale);

        // NOTE: A product of unit triangular integer matrices has determinant 1
        // and an integer inverse, and fills the last column unlike the matrices above.
        const auto lower = Matrix4x4{
            1.0f, 0.0f, 0.0f, 0.0f,
            2.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 3.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 4.0f, 1.0f};
        const auto inverseLower = Matrix4x4{
            1.0f, 0.0f, 0.0f, 0.0f,
            -2.0f, 1.0f, 0.0f, 0.0f,
            6.0f, -3.0f, 1.0f, 0.0f,
            -24.0f, 12.0f, -4.0f, 1.0f};
        const auto upper = Matrix4x4{
            1.0f, 5.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 6.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 7.0f,
            0.0f, 0.0f, 0.0f, 1.0f};
        const auto inverseUpper = Matrix4x4{
            1.0f, -5.0f, 30.0f, -210.0f,
            0.0f, 1.0f, -6.0f, 42.0f,
            0.0f, 0.0f, 1.0f, -7.0f,
            0.0f, 0.0f, 0.0f, 1.0f};
        requireSame(invert(lower), inverseLower);
        requireSame(invert(upper), inverseUpper);
        requireSame(invert(lower * upper), inverseUpper * inverseLower);
        requireSame(invert(upper * lower), inverseLower * inverseUpper);
    }
    SUBCASE("invert world, view, and projection matrices")
    {
        // NOTE: Rounding grows with the magnitudes of the products summed into
        // each element of A * B, so the residual A * B - I is bounded relative to
        // sum_k |a_ik * b_kj|. The world-view-projection matrix reaches about 6e-6.
        using pomdog::math::invert;
        const auto requireInverse = [](const Matrix4x4& a, const Matrix4x4& b) {
            for (i32 row = 0; row < 4; ++row) {
                for (i32 column = 0; column < 4; ++column) {
                    CAPTURE(row);
                    CAPTURE(column);
                    f32 sum = 0.0f;
                    f32 scale = 0.0f;
                    for (i32 k = 0; k < 4; ++k) {
                        sum += a(row, k) * b(k, column);
                        scale += std::abs(a(row, k) * b(k, column));
                    }
                    const f32 expected = (row == column) ? 1.0f : 0.0f;
                    REQUIRE(std::abs(sum - expected) <= scale * 1e-4f);
                }
            }
        };
        const auto world =
            Matrix4x4::createScale(Vector3{2.0f, 0.5f, 3.0f}) *
            Matrix4x4::createFromAxisAngle(Vector3{0.6f, 0.0f, 0.8f}, Turn32{0.1f}) *
            Matrix4x4::createTranslation(Vector3{10.0f, -20.0f, 30.0f});
        const auto view = Matrix4x4::createLookAtRH(Vector3{5.0f, 6.0f, 7.0f}, Vector3::createZero(), Vector3::createUnitY());
        const auto projection = Matrix4x4::createPerspectiveFieldOfViewRH(toRadian(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
        const auto worldViewProjection = world * view * projection;

        SUBCASE("world")
        {
            requireInverse(world, invert(world));
            requireInverse(invert(world), world);
        }
        SUBCASE("view")
        {
            requireInverse(view, invert(view));
            requireInverse(invert(view), view);
        }
        SUBCASE("projection")
        {
            requireInverse(projection, invert(projection));
            requireInverse(invert(projection), projection);
        }
        SUBCASE("world * view * projection")
        {
            requireInverse(worldViewProjection, invert(worldViewProjection));
            requireInverse(invert(worldViewProjection), worldViewProjection);
        }
    }
}

TEST_CASE("Matrix4x4::createFromQuaternion")
{
    const auto requireElements = [](const Matrix4x4& actual, const std::array<f32, 16>& expected) {
        for (i32 row = 0; row < 4; ++row) {
            for (i32 column = 0; column < 4; ++column) {
                CAPTURE(row);
                CAPTURE(column);
                REQUIRE_EQ(actual(row, column), expected[static_cast<std::size_t>(row * 4 + column)]);
            }
        }
    };
    SUBCASE("identity")
    {
        requireElements(
            Matrix4x4::createFromQuaternion(Quaternion{0.0f, 0.0f, 0.0f, 1.0f}),
            {{1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f}});
    }
    SUBCASE("every element")
    {
        // NOTE: An unnormalized quaternion whose elements are distinct small
        // integers, so each element is exact and lands in one place only.
        requireElements(
            Matrix4x4::createFromQuaternion(Quaternion{3.0f, -4.0f, 5.0f, 6.0f}),
            {{-81.0f, 36.0f, 78.0f, 0.0f,
                -84.0f, -67.0f, -4.0f, 0.0f,
                -18.0f, -76.0f, -49.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f}});
    }
    SUBCASE("rotation around an axis")
    {
        const auto axis = Vector3{0.6f, 0.0f, 0.8f};
        constexpr std::array<f32, 4> angles = {{-0.375f, 0.1f, 0.6f, 2.125f}};
        for (const f32 turns : angles) {
            CAPTURE(turns);
            const auto actual = Matrix4x4::createFromQuaternion(Quaternion::createFromAxisAngle(axis, Turn32{turns}));
            const auto expected = Matrix4x4::createFromAxisAngle(axis, Turn32{turns});
            for (i32 row = 0; row < 4; ++row) {
                for (i32 column = 0; column < 4; ++column) {
                    CAPTURE(row);
                    CAPTURE(column);
                    REQUIRE(actual(row, column) == doctest::Approx(expected(row, column)).epsilon(1e-5));
                }
            }
        }
    }
}

TEST_CASE("Matrix4x4::createLookAtLH and createLookAtRH")
{
    const auto requireElements = [](const Matrix4x4& actual, const std::array<f32, 16>& expected) {
        for (i32 row = 0; row < 4; ++row) {
            for (i32 column = 0; column < 4; ++column) {
                CAPTURE(row);
                CAPTURE(column);
                REQUIRE_EQ(actual(row, column), expected[static_cast<std::size_t>(row * 4 + column)]);
            }
        }
    };
    // NOTE: The camera looks along +x with +z up, so every axis is a unit axis
    // and every element is exact, while the rows and the translation differ
    // enough to show an element in the wrong place.
    const auto eye = Vector3{1.0f, 2.0f, 3.0f};
    const auto at = Vector3{5.0f, 2.0f, 3.0f};
    const auto up = Vector3{0.0f, 0.0f, 1.0f};
    SUBCASE("createLookAtLH")
    {
        requireElements(
            Matrix4x4::createLookAtLH(eye, at, up),
            {{0.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                -2.0f, -3.0f, -1.0f, 1.0f}});
    }
    SUBCASE("createLookAtRH")
    {
        requireElements(
            Matrix4x4::createLookAtRH(eye, at, up),
            {{0.0f, 0.0f, -1.0f, 0.0f,
                -1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                2.0f, -3.0f, 1.0f, 1.0f}});
    }
    SUBCASE("matches the vector functions")
    {
        const auto general = Vector3{-3.5f, 4.25f, 9.0f};
        const auto target = Vector3{2.0f, -1.5f, 0.75f};
        const auto zaxis = pomdog::math::normalize(target - general);
        const auto xaxis = pomdog::math::normalize(pomdog::math::cross(Vector3::createUnitY(), zaxis));
        const auto yaxis = pomdog::math::cross(zaxis, xaxis);
        const auto view = Matrix4x4::createLookAtLH(general, target, Vector3::createUnitY());
        const std::array<f32, 16> expected = {{xaxis.x, yaxis.x, zaxis.x, 0.0f,
            xaxis.y, yaxis.y, zaxis.y, 0.0f,
            xaxis.z, yaxis.z, zaxis.z, 0.0f,
            -pomdog::math::dot(xaxis, general), -pomdog::math::dot(yaxis, general), -pomdog::math::dot(zaxis, general), 1.0f}};
        for (i32 row = 0; row < 4; ++row) {
            for (i32 column = 0; column < 4; ++column) {
                CAPTURE(row);
                CAPTURE(column);
                REQUIRE(view(row, column) == approx(expected[static_cast<std::size_t>(row * 4 + column)]));
            }
        }
    }
}
