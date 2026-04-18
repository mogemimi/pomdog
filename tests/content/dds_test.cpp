// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/content/image/dds.h"
#include "pomdog/content/image/image_container.h"
#include "pomdog/gpu/pixel_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <cstring>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::decodeDDS;
using pomdog::u32;
using pomdog::u8;
using pomdog::gpu::PixelFormat;

namespace {

// NOTE: Minimal DDS file builder for testing
struct DDSPixelFormatTest final {
    u32 ByteSize = 32;
    u32 Flags = 0;
    u32 FourCC = 0;
    u32 RGBBitCount = 0;
    u32 RedBitMask = 0;
    u32 GreenBitMask = 0;
    u32 BlueBitMask = 0;
    u32 AlphaBitMask = 0;
};

struct DDSHeaderTest final {
    u32 ByteSize = 124;
    u32 Flags = 0;
    u32 PixelHeight = 0;
    u32 PixelWidth = 0;
    u32 LinearSize = 0;
    u32 Depth = 0;
    u32 MipMapCount = 0;
    u32 Reserved1[11] = {};
    DDSPixelFormatTest PixelFormat = {};
    u32 Caps = 0;
    u32 Caps2 = 0;
    u32 Caps3 = 0;
    u32 Caps4 = 0;
    u32 Reserved2 = 0;
};

constexpr u32 makeFourCC(char a, char b, char c, char d)
{
    return static_cast<u32>(a) |
           (static_cast<u32>(b) << 8) |
           (static_cast<u32>(c) << 16) |
           (static_cast<u32>(d) << 24);
}

std::vector<u8> buildDDSData(const DDSHeaderTest& header, std::size_t pixelDataSize)
{
    u32 signature = makeFourCC('D', 'D', 'S', ' ');
    std::vector<u8> data;
    data.resize(sizeof(signature) + sizeof(header) + pixelDataSize, 0);
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    std::memcpy(data.data(), &signature, sizeof(signature));
    std::memcpy(data.data() + sizeof(signature), &header, sizeof(header));
    POMDOG_CLANG_UNSAFE_BUFFER_END
    return data;
}

} // namespace

TEST_CASE("DDS.computePixelDataByteLength")
{
    SUBCASE("R8G8B8A8_UNorm 4x4 no mipmaps")
    {
        DDSHeaderTest header;
        header.PixelWidth = 4;
        header.PixelHeight = 4;
        header.MipMapCount = 1;

        // NOTE: R8G8B8A8_UNorm: DDPF_RGB | DDPF_ALPHAPIXELS
        header.PixelFormat.Flags = 0x00000041UL; // NOTE: RGB | AlphaPixels
        header.PixelFormat.RGBBitCount = 32;
        header.PixelFormat.RedBitMask = 0x000000ffUL;
        header.PixelFormat.GreenBitMask = 0x0000ff00UL;
        header.PixelFormat.BlueBitMask = 0x00ff0000UL;
        header.PixelFormat.AlphaBitMask = 0xff000000UL;

        // 4 * 4 * 4 bytes per pixel = 64 bytes
        auto dds = buildDDSData(header, 64);
        auto [image, err] = decodeDDS(dds.data(), dds.size());
        REQUIRE(err == nullptr);
        CHECK(image.format == PixelFormat::R8G8B8A8_UNorm);
        CHECK(image.pixelData.size() == 64);
    }

    SUBCASE("A8_UNorm 8x8 no mipmaps")
    {
        DDSHeaderTest header;
        header.PixelWidth = 8;
        header.PixelHeight = 8;
        header.MipMapCount = 1;

        // NOTE: Alpha only
        header.PixelFormat.Flags = 0x00000002UL; // NOTE: DDPF_ALPHA
        header.PixelFormat.RGBBitCount = 8;

        // NOTE: 8 * 8 * 1 = 64 bytes
        auto dds = buildDDSData(header, 64);
        auto [image, err] = decodeDDS(dds.data(), dds.size());
        REQUIRE(err == nullptr);
        CHECK(image.format == PixelFormat::A8_UNorm);
        CHECK(image.pixelData.size() == 64);
    }

    SUBCASE("BlockComp1_UNorm 4x4")
    {
        DDSHeaderTest header;
        header.PixelWidth = 4;
        header.PixelHeight = 4;
        header.MipMapCount = 1;
        header.PixelFormat.Flags = 0x00000004UL; // NOTE: DDPF_FOURCC
        header.PixelFormat.FourCC = makeFourCC('D', 'X', 'T', '1');

        // NOTE: 1 block of 4x4, 8 bytes per block = 8 bytes
        auto dds = buildDDSData(header, 8);
        auto [image, err] = decodeDDS(dds.data(), dds.size());
        REQUIRE(err == nullptr);
        CHECK(image.format == PixelFormat::BlockComp1_UNorm);
        CHECK(image.pixelData.size() == 8);
    }

    SUBCASE("R16G16_Float 2x2 no mipmaps")
    {
        DDSHeaderTest header;
        header.PixelWidth = 2;
        header.PixelHeight = 2;
        header.MipMapCount = 1;

        // NOTE: R16G16_Float: DDPF_RGB with 32-bit
        header.PixelFormat.Flags = 0x00000040UL; // NOTE: DDPF_RGB
        header.PixelFormat.RGBBitCount = 32;
        header.PixelFormat.RedBitMask = 0x0000ffffUL;
        header.PixelFormat.GreenBitMask = 0xffff0000UL;

        // NOTE: 2 * 2 * 4 = 16 bytes
        auto dds = buildDDSData(header, 16);
        auto [image, err] = decodeDDS(dds.data(), dds.size());
        REQUIRE(err == nullptr);
        CHECK(image.format == PixelFormat::R16G16_Float);
        CHECK(image.pixelData.size() == 16);
    }
}
