// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/Image/DDS.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Content/Utility/MakeFourCC.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using Pomdog::Detail::BinaryReader;
using Pomdog::Detail::MakeFourCC;

namespace Pomdog::DDS {
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
    std::uint32_t ByteSize;
    std::uint32_t Flags;
    std::uint32_t FourCC;
    std::uint32_t RGBBitCount;
    std::uint32_t RedBitMask;
    std::uint32_t GreenBitMask;
    std::uint32_t BlueBitMask;
    std::uint32_t AlphaBitMask;
});

POMDOG_INTERNAL_PACKED(struct DDSHeader final {
    std::uint32_t ByteSize;
    std::uint32_t Flags;
    std::uint32_t PixelHeight;
    std::uint32_t PixelWidth;
    std::uint32_t LinearSize;
    std::uint32_t Depth;
    std::uint32_t MipMapCount;
    std::uint32_t Reserved1[11];
    DDSPixelFormat PixelFormat;
    std::uint32_t Caps;
    std::uint32_t Caps2;
    std::uint32_t Caps3;
    std::uint32_t Caps4;
    std::uint32_t Reserved2;
});

POMDOG_INTERNAL_PACKED(struct DDSHeaderDXT10 final {
    std::uint32_t DxgiFormat; // Note: enum DXGI_FORMAT
    std::uint32_t ResourceDimension; // Note: enum D3D10_RESOURCE_DIMENSION
    std::uint32_t MiscFlag;
    std::uint32_t ArraySize;
    std::uint32_t Reserved;
});

namespace DirectDrawPixelFormat {

static constexpr std::uint32_t FourCC      = 0x00000004; // DDPF_FOURCC
static constexpr std::uint32_t AlphaPixels = 0x00000001; // DDPF_ALPHAPIXELS
static constexpr std::uint32_t Alpha       = 0x00000002; // DDPF_ALPHA
static constexpr std::uint32_t RGB         = 0x00000040; // DDPF_RGB
static constexpr std::uint32_t Luminance   = 0x00020000; // DDPF_LUMINANCE

} // namespace DirectDrawPixelFormat

[[nodiscard]] bool
IsDDSFormat(std::uint32_t signature) noexcept
{
    constexpr auto fourCC = MakeFourCC('D', 'D', 'S', ' ');
    static_assert(fourCC == 0x20534444, "The four character code value is 'DDS '");
    return (signature == fourCC);
}

[[nodiscard]] std::optional<SurfaceFormat>
ToSurfaceFormat(const DDSPixelFormat& pixelFormat)
{
    constexpr std::uint32_t FourCC_A32B32G32R32_Float = 0x00000074;

    if (pixelFormat.Flags & DirectDrawPixelFormat::FourCC) {
        if (pixelFormat.FourCC == MakeFourCC('D', 'X', 'T', '1')) {
            return SurfaceFormat::BlockComp1_UNorm;
        }
        else if (pixelFormat.FourCC == MakeFourCC('D', 'X', 'T', '2')) {
            return SurfaceFormat::BlockComp2_UNorm;
        }
        else if (pixelFormat.FourCC == MakeFourCC('D', 'X', 'T', '3')) {
            return SurfaceFormat::BlockComp2_UNorm;
        }
        else if (pixelFormat.FourCC == MakeFourCC('D', 'X', 'T', '4')) {
            return SurfaceFormat::BlockComp3_UNorm;
        }
        else if (pixelFormat.FourCC == MakeFourCC('D', 'X', 'T', '5')) {
            return SurfaceFormat::BlockComp3_UNorm;
        }
        else if (pixelFormat.FourCC == FourCC_A32B32G32R32_Float) {
            return SurfaceFormat::R32G32B32A32_Float;
        }
    }
    else if (pixelFormat.Flags & DirectDrawPixelFormat::RGB) {
        switch (pixelFormat.RGBBitCount) {
        case 32:
            if (
                (0x000000ff == pixelFormat.RedBitMask) &&
                (0x0000ff00 == pixelFormat.GreenBitMask) &&
                (0x00ff0000 == pixelFormat.BlueBitMask) &&
                (0xff000000 == pixelFormat.AlphaBitMask)) {
                return SurfaceFormat::R8G8B8A8_UNorm;
            }
            else if (
                (0x000000ff == pixelFormat.BlueBitMask) &&
                (0x0000ff00 == pixelFormat.GreenBitMask) &&
                (0x00ff0000 == pixelFormat.RedBitMask) &&
                (0xff000000 == pixelFormat.AlphaBitMask)) {
                return SurfaceFormat::B8G8R8A8_UNorm;
            }
            else if (
                (0x0000ffff == pixelFormat.RedBitMask) &&
                (0xffff0000 == pixelFormat.GreenBitMask)) {
                return SurfaceFormat::R16G16_Float;
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
            return SurfaceFormat::A8_UNorm;
        }
    }
    else if (pixelFormat.Flags & DirectDrawPixelFormat::Luminance) {
        switch (pixelFormat.RGBBitCount) {
        case 8:
            if (
                (0x000000ff == pixelFormat.RedBitMask) &&
                (0x00000000 == pixelFormat.GreenBitMask) &&
                (0x00000000 == pixelFormat.BlueBitMask) &&
                (0x00000000 == pixelFormat.AlphaBitMask)) {
                return SurfaceFormat::R8_UNorm;
            }
            break;
        case 16:
            if (
                (pixelFormat.Flags & DirectDrawPixelFormat::AlphaPixels) &&
                (0x000000ff == pixelFormat.RedBitMask) &&
                (0x00000000 == pixelFormat.GreenBitMask) &&
                (0x00000000 == pixelFormat.BlueBitMask) &&
                (0x0000ff00 == pixelFormat.AlphaBitMask)) {
                return SurfaceFormat::R8G8_UNorm;
            }
            break;
        default:
            break;
        }
    }

    return std::nullopt;
}

[[nodiscard]] std::size_t
ComputePixelDataByteLength(const DDSHeader& ddsHeader, SurfaceFormat format)
{
    const auto levelCount = (ddsHeader.MipMapCount > 0) ? ddsHeader.MipMapCount : 1;

    const auto bytesPerBlock = [&format]() -> int {
        switch (format) {
        case SurfaceFormat::BlockComp1_UNorm:
            return 8;
        case SurfaceFormat::BlockComp2_UNorm:
            return 16;
        case SurfaceFormat::BlockComp3_UNorm:
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
        std::size_t const strideBytesPerMipMap = ((mipmapPixelWidth + 3) / 4) * ((mipmapPixelHeight + 3) / 4) * bytesPerBlock;

        result += strideBytesPerMipMap;

        mipmapPixelWidth = (mipmapPixelWidth >> 1) ? (mipmapPixelWidth >> 1) : 1;
        mipmapPixelHeight = (mipmapPixelHeight >> 1) ? (mipmapPixelHeight >> 1) : 1;
    }
    return result;
}

} // namespace

std::tuple<ImageBuffer, std::unique_ptr<Error>>
Decode(const std::uint8_t* data, std::size_t size)
{
    POMDOG_ASSERT(data != nullptr);
    POMDOG_ASSERT(size > 0);

    ImageBuffer image;
    image.PixelData = nullptr;
    image.ByteLength = 0;

    std::size_t offsetBytes = 0;

    if (!BinaryReader::CanRead<std::uint32_t>(size - offsetBytes)) {
        return std::make_tuple(std::move(image), Errors::New("cannot find dds signature"));
    }
    const auto ddsSignature = BinaryReader::Read<std::uint32_t>(data + offsetBytes);
    offsetBytes += sizeof(ddsSignature);

    if (!IsDDSFormat(ddsSignature)) {
        return std::make_tuple(std::move(image), Errors::New("invalid format"));
    }

    if (!BinaryReader::CanRead<DDSHeader>(size - offsetBytes)) {
        return std::make_tuple(std::move(image), Errors::New("dds header has an invalid format"));
    }
    const auto ddsHeader = BinaryReader::Read<DDSHeader>(data + offsetBytes);
    offsetBytes += sizeof(ddsHeader);

    if (ddsHeader.ByteSize != sizeof(DDSHeader)) {
        return std::make_tuple(std::move(image), Errors::New("dds header has an invalid format"));
    }
    if (ddsHeader.PixelFormat.ByteSize != sizeof(DDSPixelFormat)) {
        return std::make_tuple(std::move(image), Errors::New("dds header has an invalid format"));
    }

    bool hasDXT10Header = false;

    if ((ddsHeader.PixelFormat.Flags & DirectDrawPixelFormat::FourCC) &&
        (MakeFourCC('D', 'X', '1', '0') == ddsHeader.PixelFormat.FourCC)) {
        hasDXT10Header = true;
    }

    if (hasDXT10Header) {
        // FIXME: Not implemented yet.
        return std::make_tuple(std::move(image), Errors::New("Sorry, DXT10 header is not supported yet."));
    }

    image.Width = ddsHeader.PixelWidth;
    image.Height = ddsHeader.PixelHeight;
    image.MipmapCount = ddsHeader.MipMapCount;

    if (auto format = ToSurfaceFormat(ddsHeader.PixelFormat); format != std::nullopt) {
        image.Format = *format;
    }
    else {
        return std::make_tuple(std::move(image), Errors::New("cannot find the surface format. Undefined or not supported"));
    }
    image.ByteLength = ComputePixelDataByteLength(ddsHeader, image.Format);

    if ((size - offsetBytes) < image.ByteLength) {
        return std::make_tuple(std::move(image), Errors::New("dds header has an invalid format"));
    }
    image.PixelData = data + offsetBytes;

    POMDOG_ASSERT(image.RawData.empty());

    return std::make_tuple(std::move(image), nullptr);
}

} // namespace Pomdog::DDS
