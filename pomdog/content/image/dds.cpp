// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/image/dds.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/image/image_container.h"
#include "pomdog/content/utility/binary_reader.h"
#include "pomdog/content/utility/make_fourcc.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace BinaryReader = pomdog::detail::BinaryReader;
using pomdog::detail::makeFourCC;

namespace pomdog {
namespace {

#if defined(POMDOG_INTERNAL_PACKED)
#error "'POMDOG_INTERNAL_PACKED' already defined."
#elif defined(_MSC_VER)
#define POMDOG_INTERNAL_PACKED(type) __pragma(pack(push, 1)) type __pragma(pack(pop))
#elif defined(__clang__) || defined(__GNUC__)
#define POMDOG_INTERNAL_PACKED(type) type __attribute__((packed))
#else
#error "'POMDOG_INTERNAL_PACKED' is not supported in this compiler."
#endif

POMDOG_INTERNAL_PACKED(struct DDSPixelFormat final {
    u32 ByteSize;
    u32 Flags;
    u32 FourCC;
    u32 RGBBitCount;
    u32 RedBitMask;
    u32 GreenBitMask;
    u32 BlueBitMask;
    u32 AlphaBitMask;
});

POMDOG_INTERNAL_PACKED(struct DDSHeader final {
    u32 ByteSize;
    u32 Flags;
    u32 PixelHeight;
    u32 PixelWidth;
    u32 LinearSize;
    u32 Depth;
    u32 MipMapCount;
    u32 Reserved1[11];
    DDSPixelFormat PixelFormat;
    u32 Caps;
    u32 Caps2;
    u32 Caps3;
    u32 Caps4;
    u32 Reserved2;
});

POMDOG_INTERNAL_PACKED(struct DDSHeaderDXT10 final {
    u32 DxgiFormat;        // NOTE: enum DXGI_FORMAT
    u32 ResourceDimension; // NOTE: enum D3D10_RESOURCE_DIMENSION
    u32 MiscFlag;
    u32 ArraySize;
    u32 Reserved;
});

namespace DirectDrawPixelFormat {

constexpr u32 FourCC = 0x00000004UL;      // DDPF_FOURCC
constexpr u32 AlphaPixels = 0x00000001UL; // DDPF_ALPHAPIXELS
constexpr u32 Alpha = 0x00000002UL;       // DDPF_ALPHA
constexpr u32 RGB = 0x00000040UL;         // DDPF_RGB
constexpr u32 Luminance = 0x00020000UL;   // DDPF_LUMINANCE

} // namespace DirectDrawPixelFormat

[[nodiscard]] bool
isDDSFormat(u32 signature) noexcept
{
    constexpr auto fourCC = makeFourCC('D', 'D', 'S', ' ');
    static_assert(fourCC == 0x20534444, "The four character code value is 'DDS '");
    return (signature == fourCC);
}

[[nodiscard]] std::optional<gpu::PixelFormat>
toPixelFormat(const DDSPixelFormat& pixelFormat)
{
    constexpr u32 FourCC_A32B32G32R32_Float = 0x00000074UL;

    if (pixelFormat.Flags & DirectDrawPixelFormat::FourCC) {
        if (pixelFormat.FourCC == makeFourCC('D', 'X', 'T', '1')) {
            return gpu::PixelFormat::BlockComp1_UNorm;
        }
        else if (pixelFormat.FourCC == makeFourCC('D', 'X', 'T', '2')) {
            return gpu::PixelFormat::BlockComp2_UNorm;
        }
        else if (pixelFormat.FourCC == makeFourCC('D', 'X', 'T', '3')) {
            return gpu::PixelFormat::BlockComp2_UNorm;
        }
        else if (pixelFormat.FourCC == makeFourCC('D', 'X', 'T', '4')) {
            return gpu::PixelFormat::BlockComp3_UNorm;
        }
        else if (pixelFormat.FourCC == makeFourCC('D', 'X', 'T', '5')) {
            return gpu::PixelFormat::BlockComp3_UNorm;
        }
        else if (pixelFormat.FourCC == FourCC_A32B32G32R32_Float) {
            return gpu::PixelFormat::R32G32B32A32_Float;
        }
    }
    else if (pixelFormat.Flags & DirectDrawPixelFormat::RGB) {
        switch (pixelFormat.RGBBitCount) {
        case 32:
            if (
                (u32(0x000000ffUL) == pixelFormat.RedBitMask) &&
                (u32(0x0000ff00UL) == pixelFormat.GreenBitMask) &&
                (u32(0x00ff0000UL) == pixelFormat.BlueBitMask) &&
                (u32(0xff000000UL) == pixelFormat.AlphaBitMask)) {
                return gpu::PixelFormat::R8G8B8A8_UNorm;
            }
            else if (
                (u32(0x000000ffUL) == pixelFormat.BlueBitMask) &&
                (u32(0x0000ff00UL) == pixelFormat.GreenBitMask) &&
                (u32(0x00ff0000UL) == pixelFormat.RedBitMask) &&
                (u32(0xff000000UL) == pixelFormat.AlphaBitMask)) {
                return gpu::PixelFormat::B8G8R8A8_UNorm;
            }
            else if (
                (u32(0x0000ffffUL) == pixelFormat.RedBitMask) &&
                (u32(0xffff0000UL) == pixelFormat.GreenBitMask)) {
                return gpu::PixelFormat::R16G16_Float;
            }
            break;
        case 24:
        case 16:
            break;
        default:
            break;
        }
    }
    else if (pixelFormat.Flags & DirectDrawPixelFormat::Alpha) {
        if (8 == pixelFormat.RGBBitCount) {
            return gpu::PixelFormat::A8_UNorm;
        }
    }
    else if (pixelFormat.Flags & DirectDrawPixelFormat::Luminance) {
        switch (pixelFormat.RGBBitCount) {
        case 8:
            if (
                (u32(0x000000ffUL) == pixelFormat.RedBitMask) &&
                (u32(0x00000000UL) == pixelFormat.GreenBitMask) &&
                (u32(0x00000000UL) == pixelFormat.BlueBitMask) &&
                (u32(0x00000000UL) == pixelFormat.AlphaBitMask)) {
                return gpu::PixelFormat::R8_UNorm;
            }
            break;
        case 16:
            if (
                (pixelFormat.Flags & DirectDrawPixelFormat::AlphaPixels) &&
                (u32(0x000000ffUL) == pixelFormat.RedBitMask) &&
                (u32(0x00000000UL) == pixelFormat.GreenBitMask) &&
                (u32(0x00000000UL) == pixelFormat.BlueBitMask) &&
                (u32(0x0000ff00UL) == pixelFormat.AlphaBitMask)) {
                return gpu::PixelFormat::R8G8_UNorm;
            }
            break;
        default:
            break;
        }
    }

    return std::nullopt;
}

[[nodiscard]] std::size_t
computePixelDataByteLength(const DDSHeader& ddsHeader, gpu::PixelFormat format)
{
    const auto levelCount = (ddsHeader.MipMapCount > 0) ? ddsHeader.MipMapCount : 1;

    const auto bytesPerBlock = [&format]() -> int {
        switch (format) {
        case gpu::PixelFormat::BlockComp1_UNorm:
            return 8;
        case gpu::PixelFormat::BlockComp2_UNorm:
            return 16;
        case gpu::PixelFormat::BlockComp3_UNorm:
            return 16;
        default:
            break;
        }
        return 16;
    }();

    std::size_t result = 0;
    std::size_t mipmapPixelWidth = ddsHeader.PixelWidth;
    std::size_t mipmapPixelHeight = ddsHeader.PixelHeight;

    for (std::uint32_t mipmapLevel = 0; mipmapLevel < levelCount; ++mipmapLevel) {
        std::size_t const strideBytesPerMipMap =
            ((mipmapPixelWidth + 3) / 4) *
            ((mipmapPixelHeight + 3) / 4) *
            static_cast<std::size_t>(bytesPerBlock);

        result += strideBytesPerMipMap;

        mipmapPixelWidth = (mipmapPixelWidth >> 1) ? (mipmapPixelWidth >> 1) : 1;
        mipmapPixelHeight = (mipmapPixelHeight >> 1) ? (mipmapPixelHeight >> 1) : 1;
    }
    return result;
}

} // namespace

[[nodiscard]] std::tuple<ImageContainer, std::unique_ptr<Error>>
decodeDDS(const u8* data, std::size_t size)
{
    POMDOG_ASSERT(data != nullptr);
    POMDOG_ASSERT(size > 0);

    ImageContainer image = {};

    std::size_t offsetBytes = 0;

    if (!BinaryReader::canRead<std::uint32_t>(size - offsetBytes)) {
        return std::make_tuple(std::move(image), errors::make("cannot find dds signature"));
    }
    const auto ddsSignature = BinaryReader::read<std::uint32_t>(data + offsetBytes);
    offsetBytes += sizeof(ddsSignature);

    if (!isDDSFormat(ddsSignature)) {
        return std::make_tuple(std::move(image), errors::make("invalid format"));
    }

    if (!BinaryReader::canRead<DDSHeader>(size - offsetBytes)) {
        return std::make_tuple(std::move(image), errors::make("dds header has an invalid format"));
    }
    const auto ddsHeader = BinaryReader::read<DDSHeader>(data + offsetBytes);
    offsetBytes += sizeof(ddsHeader);

    if (ddsHeader.ByteSize != sizeof(DDSHeader)) {
        return std::make_tuple(std::move(image), errors::make("dds header has an invalid format"));
    }
    if (ddsHeader.PixelFormat.ByteSize != sizeof(DDSPixelFormat)) {
        return std::make_tuple(std::move(image), errors::make("dds header has an invalid format"));
    }

    bool hasDXT10Header = false;

    if ((ddsHeader.PixelFormat.Flags & DirectDrawPixelFormat::FourCC) &&
        (makeFourCC('D', 'X', '1', '0') == ddsHeader.PixelFormat.FourCC)) {
        hasDXT10Header = true;
    }

    if (hasDXT10Header) {
        // FIXME: Not implemented yet.
        return std::make_tuple(std::move(image), errors::make("Sorry, DXT10 header is not supported yet."));
    }

    image.width = static_cast<i32>(ddsHeader.PixelWidth);
    image.height = static_cast<i32>(ddsHeader.PixelHeight);
    image.mipmapCount = static_cast<i32>(ddsHeader.MipMapCount);

    if (auto format = toPixelFormat(ddsHeader.PixelFormat); format != std::nullopt) {
        image.format = *format;
    }
    else {
        return std::make_tuple(std::move(image), errors::make("cannot find the surface format. Undefined or not supported"));
    }
    const auto byteLength = computePixelDataByteLength(ddsHeader, image.format);

    if ((size - offsetBytes) < byteLength) {
        return std::make_tuple(std::move(image), errors::make("dds header has an invalid format"));
    }

    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    image.pixelData = std::span<const u8>{data + offsetBytes, byteLength};
    POMDOG_CLANG_UNSAFE_BUFFER_END

    // NOTE: We don't copy pixel data to `image.rawData` to avoid extra memory allocation.
    POMDOG_ASSERT(image.rawData.empty());

    return std::make_tuple(std::move(image), nullptr);
}

} // namespace pomdog
