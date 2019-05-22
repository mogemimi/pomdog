// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "catch.hpp"

using Pomdog::Color;
using Pomdog::Vector3;
using Pomdog::Vector4;

TEST_CASE("Color", "[Color]")
{
    SECTION("FirstTestCase")
    {
        Color color{255, 255, 255, 255};
        REQUIRE(color.R == 255);
        REQUIRE(color.G == 255);
        REQUIRE(color.B == 255);
        REQUIRE(color.A == 255);

        color = Pomdog::Color{0, 0, 0, 0};
        REQUIRE(color.R == 0);
        REQUIRE(color.G == 0);
        REQUIRE(color.B == 0);
        REQUIRE(color.A == 0);

        color = Pomdog::Color{1, 2, 3, 4};
        REQUIRE(color.R == 1);
        REQUIRE(color.G == 2);
        REQUIRE(color.B == 3);
        REQUIRE(color.A == 4);
    }
    SECTION("Constants")
    {
        REQUIRE(Color::Black == Color{0, 0, 0, 255});
        REQUIRE(Color::White == Color{255, 255, 255, 255});
        REQUIRE(Color::Red == Color{255, 0, 0, 255});
        REQUIRE(Color::Green == Color{0, 255, 0, 255});
        REQUIRE(Color::Blue == Color{0, 0, 255, 255});
        REQUIRE(Color::Yellow == Color{255, 255, 0, 255});
        REQUIRE(Color::TransparentBlack == Color{0, 0, 0, 0});
        REQUIRE(Color::CornflowerBlue == Color{100, 149, 237, 255});
    }
    SECTION("ToVector")
    {
        Color color{255, 255, 255, 255};
        REQUIRE(Vector3{1.0f, 1.0f, 1.0f} == color.ToVector3());
        REQUIRE(Vector4{1.0f, 1.0f, 1.0f, 1.0f} == color.ToVector4());

        color = Color{0, 0, 0, 0};
        REQUIRE(Vector3{0.0f, 0.0f, 0.0f} == color.ToVector3());
        REQUIRE(Vector4{0.0f, 0.0f, 0.0f, 0.0f} == color.ToVector4());

        color = Color{12, 34, 56, 78};
        REQUIRE(std::abs(12.0f/255.0f - color.ToVector3().X) < std::numeric_limits<float>::epsilon());
        REQUIRE(std::abs(34.0f/255.0f - color.ToVector3().Y) < std::numeric_limits<float>::epsilon());
        REQUIRE(std::abs(56.0f/255.0f - color.ToVector3().Z) < std::numeric_limits<float>::epsilon());
        REQUIRE(std::abs(12.0f/255.0f - color.ToVector4().X) < std::numeric_limits<float>::epsilon());
        REQUIRE(std::abs(34.0f/255.0f - color.ToVector4().Y) < std::numeric_limits<float>::epsilon());
        REQUIRE(std::abs(56.0f/255.0f - color.ToVector4().Z) < std::numeric_limits<float>::epsilon());
        REQUIRE(std::abs(78.0f/255.0f - color.ToVector4().W) < std::numeric_limits<float>::epsilon());
    }
    SECTION("FromVector")
    {
        REQUIRE(Color{0, 0, 0, 255} == Color(Vector3{0.0f, 0.0f, 0.0f}));
        REQUIRE(Color{255, 255, 255, 255} == Color(Vector3{1.0f, 1.0f, 1.0f}));

        REQUIRE(Color{0, 0, 0, 0} == Color(Vector4{0.0f, 0.0f, 0.0f, 0.0f}));
        REQUIRE(Color{255, 255, 255, 255} == Color(Vector4{1.0f, 1.0f, 1.0f, 1.0f}));

        REQUIRE(Color{255, 0, 0, 255} == Color(Vector3{1.0f, 0.0f, 0.0f}));
        REQUIRE(Color{0, 255, 0, 255} == Color(Vector3{0.0f, 1.0f, 0.0f}));
        REQUIRE(Color{0, 0, 255, 255} == Color(Vector3{0.0f, 0.0f, 1.0f}));

        REQUIRE(Color{255, 0, 0, 0} == Color(Vector4{1.0f, 0.0f, 0.0f, 0.0f}));
        REQUIRE(Color{0, 255, 0, 0} == Color(Vector4{0.0f, 1.0f, 0.0f, 0.0f}));
        REQUIRE(Color{0, 0, 255, 0} == Color(Vector4{0.0f, 0.0f, 1.0f, 0.0f}));
        REQUIRE(Color{0, 0, 0, 255} == Color(Vector4{0.0f, 0.0f, 0.0f, 1.0f}));

        REQUIRE(std::abs(0.3f * 255 - Color(Vector3{0.3f, 0.4f, 0.5f}).R) <= 0.5f);
        REQUIRE(std::abs(0.4f * 255 - Color(Vector3{0.3f, 0.4f, 0.5f}).G) <= 0.5f);
        REQUIRE(std::abs(0.5f * 255 - Color(Vector3{0.3f, 0.4f, 0.5f}).B) <= 0.5f);

        REQUIRE(std::abs(0.3f * 255 - Color(Vector4{0.3f, 0.4f, 0.5f, 0.6f}).R) <= 0.5f);
        REQUIRE(std::abs(0.4f * 255 - Color(Vector4{0.3f, 0.4f, 0.5f, 0.6f}).G) <= 0.5f);
        REQUIRE(std::abs(0.5f * 255 - Color(Vector4{0.3f, 0.4f, 0.5f, 0.6f}).B) <= 0.5f);
        REQUIRE(std::abs(0.6f * 255 - Color(Vector4{0.3f, 0.4f, 0.5f, 0.6f}).A) <= 0.5f);
    }
    SECTION("FromPackedValue")
    {
        REQUIRE(Color{0, 0, 0, 0} == Color::FromPackedValue(0x00000000));
        REQUIRE(Color{255, 255, 255, 255} == Color::FromPackedValue(0xffffffff));
        REQUIRE(Color{204, 221, 238, 255} == Color::FromPackedValue(0xccddeeff));
        REQUIRE(Color{100, 149, 237, 255} == Color::FromPackedValue(0x6495edff));
        REQUIRE(Color{100, 149, 237, 153} == Color::FromPackedValue(0x6495ed99));
    }
    SECTION("Multiply")
    {
        REQUIRE(Color{255, 255, 255, 255} == Color::Multiply(Color::White, 2.0f));
        REQUIRE(Color{255, 255, 255, 255} == Color::Multiply(Color::White, 1.0f));
        REQUIRE(Color{128, 128, 128, 128} == Color::Multiply(Color::White, 0.5f));
        REQUIRE(Color{64, 64, 64, 64} == Color::Multiply(Color::White, 0.25f));
        REQUIRE(Color{0, 0, 0, 0} == Color::Multiply(Color::White, 0.0f));
        REQUIRE(Color{0, 0, 0, 0} == Color::Multiply(Color::TransparentBlack, 2.0f));
        REQUIRE(Color{0, 0, 0, 0} == Color::Multiply(Color::TransparentBlack, 1.0f));
        REQUIRE(Color{0, 0, 0, 0} == Color::Multiply(Color::TransparentBlack, 0.0f));
    }
}
