// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/color.h"
#include "pomdog/math/vector3.h"
#include "pomdog/math/vector4.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Color;
using pomdog::u32;
using pomdog::Vector3;
using pomdog::Vector4;

TEST_CASE("Color")
{
    SUBCASE("constructor")
    {
        Color color{255, 255, 255, 255};
        REQUIRE(color.r == 255);
        REQUIRE(color.g == 255);
        REQUIRE(color.b == 255);
        REQUIRE(color.a == 255);

        color = Color{0, 0, 0, 0};
        REQUIRE(color.r == 0);
        REQUIRE(color.g == 0);
        REQUIRE(color.b == 0);
        REQUIRE(color.a == 0);

        color = Color{1, 2, 3, 4};
        REQUIRE(color.r == 1);
        REQUIRE(color.g == 2);
        REQUIRE(color.b == 3);
        REQUIRE(color.a == 4);
    }
    SUBCASE("constants")
    {
        REQUIRE(Color::createWhite().r == 255);
        REQUIRE(Color::createWhite().g == 255);
        REQUIRE(Color::createWhite().b == 255);
        REQUIRE(Color::createWhite().a == 255);

        REQUIRE(Color::createBlack().r == 0);
        REQUIRE(Color::createBlack().g == 0);
        REQUIRE(Color::createBlack().b == 0);
        REQUIRE(Color::createBlack().a == 255);

        REQUIRE(Color::createRed().r == 255);
        REQUIRE(Color::createRed().g == 0);
        REQUIRE(Color::createRed().b == 0);
        REQUIRE(Color::createRed().a == 255);

        REQUIRE(Color::createLime().r == 0);
        REQUIRE(Color::createLime().g == 255);
        REQUIRE(Color::createLime().b == 0);
        REQUIRE(Color::createLime().a == 255);

        REQUIRE(Color::createBlue().r == 0);
        REQUIRE(Color::createBlue().g == 0);
        REQUIRE(Color::createBlue().b == 255);
        REQUIRE(Color::createBlue().a == 255);

        REQUIRE(Color::createYellow().r == 255);
        REQUIRE(Color::createYellow().g == 255);
        REQUIRE(Color::createYellow().b == 0);
        REQUIRE(Color::createYellow().a == 255);

        REQUIRE(Color::createCornflowerBlue().r == 100);
        REQUIRE(Color::createCornflowerBlue().g == 149);
        REQUIRE(Color::createCornflowerBlue().b == 237);
        REQUIRE(Color::createCornflowerBlue().a == 255);

        REQUIRE(Color::createTransparentBlack().r == 0);
        REQUIRE(Color::createTransparentBlack().g == 0);
        REQUIRE(Color::createTransparentBlack().b == 0);
        REQUIRE(Color::createTransparentBlack().a == 0);
    }
    SUBCASE("fromPackedValue")
    {
        REQUIRE(Color::fromPackedValue(0xffffffff) == Color::createWhite());
        REQUIRE(Color::fromPackedValue(0x000000ff) == Color::createBlack());
        REQUIRE(Color::fromPackedValue(0xff0000ff) == Color::createRed());
        REQUIRE(Color::fromPackedValue(0x00ff00ff) == Color::createLime());
        REQUIRE(Color::fromPackedValue(0x0000ffff) == Color::createBlue());
        REQUIRE(Color::fromPackedValue(0xffff00ff) == Color::createYellow());
        REQUIRE(Color::fromPackedValue(0x6495edff) == Color::createCornflowerBlue());
        REQUIRE(Color::fromPackedValue(0x00000000) == Color::createTransparentBlack());
        REQUIRE(Color::fromPackedValue(0x00000000) == Color{0, 0, 0, 0});
        REQUIRE(Color::fromPackedValue(0xffffffff) == Color{255, 255, 255, 255});
        REQUIRE(Color::fromPackedValue(0xccddeeff) == Color{204, 221, 238, 255});
        REQUIRE(Color::fromPackedValue(0x6495edff) == Color{100, 149, 237, 255});
        REQUIRE(Color::fromPackedValue(0x6495ed99) == Color{100, 149, 237, 153});
    }
    SUBCASE("toPackedValue")
    {
        REQUIRE(Color::createWhite().toPackedValue() == u32(0xffffffff));
        REQUIRE(Color::createBlack().toPackedValue() == u32(0x000000ff));
        REQUIRE(Color::createRed().toPackedValue() == u32(0xff0000ff));
        REQUIRE(Color::createLime().toPackedValue() == u32(0x00ff00ff));
        REQUIRE(Color::createBlue().toPackedValue() == u32(0x0000ffff));
        REQUIRE(Color::createYellow().toPackedValue() == u32(0xffff00ff));
        REQUIRE(Color::createCornflowerBlue().toPackedValue() == u32(0x6495edff));
        REQUIRE(Color::createTransparentBlack().toPackedValue() == u32(0x00000000));
    }
    SUBCASE("toVector3")
    {
        REQUIRE(Color{255, 255, 255, 255}.toVector3() == Vector3{1.0f, 1.0f, 1.0f});
        REQUIRE(Color{0, 0, 0, 0}.toVector3() == Vector3{0.0f, 0.0f, 0.0f});
        REQUIRE(Color{12, 34, 56, 78}.toVector3().x == doctest::Approx(12.0 / 255.0).epsilon(0.0000001));
        REQUIRE(Color{12, 34, 56, 78}.toVector3().y == doctest::Approx(34.0 / 255.0).epsilon(0.0000001));
        REQUIRE(Color{12, 34, 56, 78}.toVector3().z == doctest::Approx(56.0 / 255.0).epsilon(0.0000001));
    }
    SUBCASE("toVector4")
    {
        REQUIRE(Color{255, 255, 255, 255}.toVector4() == Vector4{1.0f, 1.0f, 1.0f, 1.0f});
        REQUIRE(Color{0, 0, 0, 0}.toVector4() == Vector4{0.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(Color{12, 34, 56, 78}.toVector4().x == doctest::Approx(12.0 / 255.0).epsilon(0.0000001));
        REQUIRE(Color{12, 34, 56, 78}.toVector4().y == doctest::Approx(34.0 / 255.0).epsilon(0.0000001));
        REQUIRE(Color{12, 34, 56, 78}.toVector4().z == doctest::Approx(56.0 / 255.0).epsilon(0.0000001));
        REQUIRE(Color{12, 34, 56, 78}.toVector4().w == doctest::Approx(78.0 / 255.0).epsilon(0.0000001));
    }
    SUBCASE("lerp")
    {
        using pomdog::math::lerp;
        REQUIRE(lerp(Color{0, 0, 0, 0}, Color{255, 255, 255, 255}, 0.0f) == Color{0, 0, 0, 0});
        REQUIRE(lerp(Color{0, 0, 0, 0}, Color{255, 255, 255, 255}, 1.0f) == Color{255, 255, 255, 255});
        REQUIRE(lerp(Color{0, 0, 0, 0}, Color{100, 100, 100, 100}, 0.5f) == Color{50, 50, 50, 50});
    }
    SUBCASE("multiply")
    {
        using pomdog::math::multiply;
        REQUIRE(multiply(Color::createWhite(), 0.0f) == Color{0, 0, 0, 0});
        REQUIRE(multiply(Color::createWhite(), 0.5f) == Color{128, 128, 128, 128});
        REQUIRE(multiply(Color::createWhite(), 1.0f) == Color::createWhite());
        REQUIRE(multiply(Color::createWhite(), 2.0f) == Color{255, 255, 255, 255});
        REQUIRE(multiply(Color::createWhite(), 1.0f) == Color{255, 255, 255, 255});
        REQUIRE(multiply(Color::createWhite(), 0.5f) == Color{128, 128, 128, 128});
        REQUIRE(multiply(Color::createWhite(), 0.25f) == Color{64, 64, 64, 64});
        REQUIRE(multiply(Color::createWhite(), 0.0f) == Color{0, 0, 0, 0});
        REQUIRE(multiply(Color::createTransparentBlack(), 0.0f) == Color{0, 0, 0, 0});
        REQUIRE(multiply(Color::createTransparentBlack(), 1.0f) == Color{0, 0, 0, 0});
        REQUIRE(multiply(Color::createTransparentBlack(), 2.0f) == Color{0, 0, 0, 0});
    }
}
