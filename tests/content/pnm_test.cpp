// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/content/image/image_container.h"
#include "pomdog/content/image/pnm.h"
#include "pomdog/gpu/pixel_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::decodePNM;
using pomdog::gpu::PixelFormat;

TEST_CASE("PNM.P4_BinaryBitmap_RowPadding")
{
    SUBCASE("Width 10, Height 2 with row padding")
    {
        // NOTE: P4 format: width=10, height=2
        // Each row is ceil(10/8) = 2 bytes
        // - Row 1: 0xFF 0xC0 = 11111111 11000000 (first 10 bits used, last 6 are padding)
        // - Row 2: 0xAA 0x80 = 10101010 10000000 (first 10 bits used, last 6 are padding)
        std::string data = "P4\n10 2\n";

        // NOTE: Row 1: all 10 pixels set
        data += static_cast<char>(0xFF);
        data += static_cast<char>(0xC0);

        // NOTE: Row 2: alternating pattern 1010101010
        data += static_cast<char>(0xAA);
        data += static_cast<char>(0x80);

        auto [image, err] = decodePNM(data.data(), data.size());
        REQUIRE(err == nullptr);
        CHECK(image.width == 10);
        CHECK(image.height == 2);
        CHECK(image.format == PixelFormat::R8_UNorm);

        // NOTE: In PNM bitmap, 1=ink(black), but the code inverts (255-c),
        // so after inversion: 1 bit -> 255 -> 0 (black), 0 bit -> 0 -> 255 (white)
        // After maxLuma==1 scaling: each raw bit is multiplied by 255
        // Then the inversion: c = 255 - c
        // So: original bit 1 -> raw=1 -> *255=255 -> 255-255=0
        //     original bit 0 -> raw=0 -> *255=0   -> 255-0=255

        // NOTE: Total pixels should be exactly width * height = 20
        REQUIRE(image.rawData.size() == 20);

        // NOTE: Row 1: 10 pixels, bits = 1111111111 -> all should be 0 (black)
        for (std::size_t i = 0; i < 10; ++i) {
            CHECK(image.rawData[i] == 0);
        }

        // NOTE: Row 2: bits = 1010101010 -> alternating 0, 255
        CHECK(image.rawData[10] == 0);   // NOTE: bit 1
        CHECK(image.rawData[11] == 255); // NOTE: bit 0
        CHECK(image.rawData[12] == 0);   // NOTE: bit 1
        CHECK(image.rawData[13] == 255); // NOTE: bit 0
        CHECK(image.rawData[14] == 0);   // NOTE: bit 1
        CHECK(image.rawData[15] == 255); // NOTE: bit 0
        CHECK(image.rawData[16] == 0);   // NOTE: bit 1
        CHECK(image.rawData[17] == 255); // NOTE: bit 0
        CHECK(image.rawData[18] == 0);   // NOTE: bit 1
        CHECK(image.rawData[19] == 255); // NOTE: bit 0
    }

    SUBCASE("Width 8, Height 1 - exact byte boundary")
    {
        // NOTE: Width=8, 1 byte per row, no padding needed
        std::string data = "P4\n8 1\n";
        data += static_cast<char>(0xAA); // NOTE: 10101010

        auto [image, err] = decodePNM(data.data(), data.size());
        REQUIRE(err == nullptr);
        CHECK(image.width == 8);
        CHECK(image.height == 1);
        REQUIRE(image.rawData.size() == 8);

        // NOTE: bit pattern: 1 0 1 0 1 0 1 0
        // NOTE: After scaling and inversion: 1->0, 0->255
        CHECK(image.rawData[0] == 0);
        CHECK(image.rawData[1] == 255);
        CHECK(image.rawData[2] == 0);
        CHECK(image.rawData[3] == 255);
        CHECK(image.rawData[4] == 0);
        CHECK(image.rawData[5] == 255);
        CHECK(image.rawData[6] == 0);
        CHECK(image.rawData[7] == 255);
    }

    SUBCASE("Width 1, Height 1 - single pixel")
    {
        // NOTE: Width=1, 1 byte per row, 7 bits padding
        std::string data = "P4\n1 1\n";
        data += static_cast<char>(0x80); // NOTE: 10000000 - first bit is 1

        auto [image, err] = decodePNM(data.data(), data.size());
        REQUIRE(err == nullptr);
        CHECK(image.width == 1);
        CHECK(image.height == 1);
        REQUIRE(image.rawData.size() == 1);
        CHECK(image.rawData[0] == 0); // NOTE: bit 1 -> 0 after inversion
    }
}
