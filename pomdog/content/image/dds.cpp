// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/image/dds.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/image/image_container.h"
#include "pomdog/content/utility/binary_reader.h"
#include "pomdog/content/utility/make_fourcc.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <optional>
#include <span>
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
    u32 byteSize;
    u32 flags;
    u32 fourCC;
    u32 rgbBitCount;
    u32 redBitMask;
    u32 greenBitMask;
    u32 blueBitMask;
    u32 alphaBitMask;
});

POMDOG_INTERNAL_PACKED(struct DDSHeader final {
    u32 byteSize;
    u32 flags;
    u32 pixelHeight;
    u32 pixelWidth;
    u32 linearSize;
    u32 depth;
    u32 mipMapCount;
    u32 reserved1[11];
    DDSPixelFormat pixelFormat;
    u32 caps;
    u32 caps2;
    u32 caps3;
    u32 caps4;
    u32 reserved2;
});

POMDOG_INTERNAL_PACKED(struct DDSHeaderDXT10 final {
    u32 dxgiFormat;        // NOTE: enum DXGI_FORMAT
    u32 resourceDimension; // NOTE: enum D3D10_RESOURCE_DIMENSION
    u32 miscFlag;
    u32 arraySize;
    u32 reserved;
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

    if (pixelFormat.flags & DirectDrawPixelFormat::FourCC) {
        if (pixelFormat.fourCC == makeFourCC('D', 'X', 'T', '1')) {
            return gpu::PixelFormat::BlockComp1_UNorm;
        }
        else if (pixelFormat.fourCC == makeFourCC('D', 'X', 'T', '2')) {
            return gpu::PixelFormat::BlockComp2_UNorm;
        }
        else if (pixelFormat.fourCC == makeFourCC('D', 'X', 'T', '3')) {
            return gpu::PixelFormat::BlockComp2_UNorm;
        }
        else if (pixelFormat.fourCC == makeFourCC('D', 'X', 'T', '4')) {
            return gpu::PixelFormat::BlockComp3_UNorm;
        }
        else if (pixelFormat.fourCC == makeFourCC('D', 'X', 'T', '5')) {
            return gpu::PixelFormat::BlockComp3_UNorm;
        }
        else if (pixelFormat.fourCC == FourCC_A32B32G32R32_Float) {
            return gpu::PixelFormat::R32G32B32A32_Float;
        }
    }
    else if (pixelFormat.flags & DirectDrawPixelFormat::RGB) {
        switch (pixelFormat.rgbBitCount) {
        case 32:
            if (
                (u32(0x000000ffUL) == pixelFormat.redBitMask) &&
                (u32(0x0000ff00UL) == pixelFormat.greenBitMask) &&
                (u32(0x00ff0000UL) == pixelFormat.blueBitMask) &&
                (u32(0xff000000UL) == pixelFormat.alphaBitMask)) {
                return gpu::PixelFormat::R8G8B8A8_UNorm;
            }
            else if (
                (u32(0x000000ffUL) == pixelFormat.blueBitMask) &&
                (u32(0x0000ff00UL) == pixelFormat.greenBitMask) &&
                (u32(0x00ff0000UL) == pixelFormat.redBitMask) &&
                (u32(0xff000000UL) == pixelFormat.alphaBitMask)) {
                return gpu::PixelFormat::B8G8R8A8_UNorm;
            }
            else if (
                (u32(0x0000ffffUL) == pixelFormat.redBitMask) &&
                (u32(0xffff0000UL) == pixelFormat.greenBitMask)) {
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
    else if (pixelFormat.flags & DirectDrawPixelFormat::Alpha) {
        if (8 == pixelFormat.rgbBitCount) {
            return gpu::PixelFormat::A8_UNorm;
        }
    }
    else if (pixelFormat.flags & DirectDrawPixelFormat::Luminance) {
        switch (pixelFormat.rgbBitCount) {
        case 8:
            if (
                (u32(0x000000ffUL) == pixelFormat.redBitMask) &&
                (u32(0x00000000UL) == pixelFormat.greenBitMask) &&
                (u32(0x00000000UL) == pixelFormat.blueBitMask) &&
                (u32(0x00000000UL) == pixelFormat.alphaBitMask)) {
                return gpu::PixelFormat::R8_UNorm;
            }
            break;
        case 16:
            if (
                (pixelFormat.flags & DirectDrawPixelFormat::AlphaPixels) &&
                (u32(0x000000ffUL) == pixelFormat.redBitMask) &&
                (u32(0x00000000UL) == pixelFormat.greenBitMask) &&
                (u32(0x00000000UL) == pixelFormat.blueBitMask) &&
                (u32(0x0000ff00UL) == pixelFormat.alphaBitMask)) {
                return gpu::PixelFormat::R8G8_UNorm;
            }
            break;
        default:
            break;
        }
    }

    return std::nullopt;
}

[[nodiscard]] bool
isBlockCompressed(gpu::PixelFormat format) noexcept
{
    switch (format) {
    case gpu::PixelFormat::BlockComp1_UNorm:
    case gpu::PixelFormat::BlockComp2_UNorm:
    case gpu::PixelFormat::BlockComp3_UNorm:
        return true;
    default:
        return false;
    }
}

[[nodiscard]] i32
getBytesPerBlock(gpu::PixelFormat format) noexcept
{
    switch (format) {
    case gpu::PixelFormat::BlockComp1_UNorm:
        return 8;
    case gpu::PixelFormat::BlockComp2_UNorm:
    case gpu::PixelFormat::BlockComp3_UNorm:
        return 16;
    default:
        return 0;
    }
}

[[nodiscard]] i32
getBytesPerPixel(gpu::PixelFormat format) noexcept
{
    switch (format) {
    case gpu::PixelFormat::A8_UNorm:
    case gpu::PixelFormat::R8_UNorm:
        return 1;
    case gpu::PixelFormat::R8G8_UNorm:
        return 2;
    case gpu::PixelFormat::R8G8B8A8_UNorm:
    case gpu::PixelFormat::B8G8R8A8_UNorm:
    case gpu::PixelFormat::R10G10B10A2_UNorm:
    case gpu::PixelFormat::R11G11B10_Float:
    case gpu::PixelFormat::R16G16_Float:
    case gpu::PixelFormat::R32_Float:
        return 4;
    case gpu::PixelFormat::R16G16B16A16_Float:
        return 8;
    case gpu::PixelFormat::R32G32B32A32_Float:
        return 16;
    default:
        return 0;
    }
}

[[nodiscard]] std::size_t
computePixelDataByteLength(const DDSHeader& ddsHeader, gpu::PixelFormat format)
{
    const auto levelCount = (ddsHeader.mipMapCount > 0) ? ddsHeader.mipMapCount : 1;

    std::size_t result = 0;
    std::size_t mipmapPixelWidth = ddsHeader.pixelWidth;
    std::size_t mipmapPixelHeight = ddsHeader.pixelHeight;
    if (isBlockCompressed(format)) {
        const auto bytesPerBlock = static_cast<std::size_t>(getBytesPerBlock(format));

        for (u32 mipmapLevel = 0; mipmapLevel < levelCount; ++mipmapLevel) {
            std::size_t const strideBytesPerMipMap =
                ((mipmapPixelWidth + 3) / 4) *
                ((mipmapPixelHeight + 3) / 4) *
                bytesPerBlock;

            result += strideBytesPerMipMap;

            mipmapPixelWidth = (mipmapPixelWidth >> 1) ? (mipmapPixelWidth >> 1) : 1;
            mipmapPixelHeight = (mipmapPixelHeight >> 1) ? (mipmapPixelHeight >> 1) : 1;
        }
    }
    else {
        const auto bytesPerPixel = static_cast<std::size_t>(getBytesPerPixel(format));

        for (u32 mipmapLevel = 0; mipmapLevel < levelCount; ++mipmapLevel) {
            result += mipmapPixelWidth * mipmapPixelHeight * bytesPerPixel;

            mipmapPixelWidth = (mipmapPixelWidth >> 1) ? (mipmapPixelWidth >> 1) : 1;
            mipmapPixelHeight = (mipmapPixelHeight >> 1) ? (mipmapPixelHeight >> 1) : 1;
        }
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

    if (!BinaryReader::canRead<u32>(size - offsetBytes)) {
        return std::make_tuple(std::move(image), errors::make("cannot find dds signature"));
    }
    const auto ddsSignature = BinaryReader::read<u32>(data + offsetBytes);
    offsetBytes += sizeof(ddsSignature);

    if (!isDDSFormat(ddsSignature)) {
        return std::make_tuple(std::move(image), errors::make("invalid format"));
    }

    if (!BinaryReader::canRead<DDSHeader>(size - offsetBytes)) {
        return std::make_tuple(std::move(image), errors::make("dds header has an invalid format"));
    }
    const auto ddsHeader = BinaryReader::read<DDSHeader>(data + offsetBytes);
    offsetBytes += sizeof(ddsHeader);

    if (ddsHeader.byteSize != sizeof(DDSHeader)) {
        return std::make_tuple(std::move(image), errors::make("dds header has an invalid format"));
    }
    if (ddsHeader.pixelFormat.byteSize != sizeof(DDSPixelFormat)) {
        return std::make_tuple(std::move(image), errors::make("dds header has an invalid format"));
    }

    bool hasDXT10Header = false;

    if ((ddsHeader.pixelFormat.flags & DirectDrawPixelFormat::FourCC) &&
        (makeFourCC('D', 'X', '1', '0') == ddsHeader.pixelFormat.fourCC)) {
        hasDXT10Header = true;
    }

    if (hasDXT10Header) {
        // FIXME: Not implemented yet.
        return std::make_tuple(std::move(image), errors::make("Sorry, DXT10 header is not supported yet."));
    }

    if (ddsHeader.pixelWidth <= 0) {
        return std::make_tuple(std::move(image), errors::make("invalid image width"));
    }
    if (ddsHeader.pixelHeight <= 0) {
        return std::make_tuple(std::move(image), errors::make("invalid image height"));
    }
    if (ddsHeader.pixelWidth > static_cast<std::size_t>(std::numeric_limits<i32>::max())) {
        return std::make_tuple(std::move(image), errors::make("too large image width"));
    }
    if (ddsHeader.pixelHeight > static_cast<std::size_t>(std::numeric_limits<i32>::max())) {
        return std::make_tuple(std::move(image), errors::make("too large image height"));
    }

    image.width = static_cast<i32>(ddsHeader.pixelWidth);
    image.height = static_cast<i32>(ddsHeader.pixelHeight);
    image.mipmapCount = static_cast<i32>(ddsHeader.mipMapCount);

    if (auto format = toPixelFormat(ddsHeader.pixelFormat); format != std::nullopt) {
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
