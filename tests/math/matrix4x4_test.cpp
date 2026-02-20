// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/radian.h"
#include "pomdog/math/vector3.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Matrix4x4;
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
    SUBCASE("first test case")
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
    SUBCASE("determinant")
    {
        using pomdog::math::determinant;
        REQUIRE(
            approx(-945.0f) ==
            determinant(Matrix4x4{
                3.0f, 4.0f, 9.0f, 2.0f,
                1.0f, 3.0f, 3.0f, 7.0f,
                5.0f, 2.0f, -4.0f, 1.0f,
                9.0f, 4.0f, 3.0f, 5.0f}));

        REQUIRE(
            approx(1.0f) ==
            determinant(Matrix4x4::createIdentity()));
    }
    SUBCASE("adjoint")
    {
        using pomdog::math::adjoint;
        REQUIRE(
            Matrix4x4::createIdentity() ==
            adjoint(Matrix4x4::createIdentity()));
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
        auto matrix = Matrix4x4::createRotationX(3.0f);
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
        auto matrix = Matrix4x4::createRotationY(3.0f);
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
        auto matrix = Matrix4x4::createRotationZ(3.0f);
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
