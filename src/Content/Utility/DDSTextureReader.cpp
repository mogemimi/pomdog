// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "DDSTextureReader.hpp"
#include "../../Basic/ConditionalCompilation.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Content/Utility/MakeFourCC.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog {
namespace Detail {
namespace {

#if defined(POMDOG_COMPILER_MSVC)
#pragma pack(push, 1)
#endif

#if defined(POMDOG_DETAIL_PACKED)
#    error "'POMDOG_DETAIL_PACKED' already defined."
#endif

#if defined(POMDOG_COMPILER_CLANG) || defined(POMDOG_COMPILER_GNUC)
#    define POMDOG_DETAIL_PACKED __attribute__((packed))
#elif defined(POMDOG_COMPILER_MSVC)
#    define POMDOG_DETAIL_PACKED
#else
#    error "'POMDOG_DETAIL_PACKED' is not supported in this compiler."
#endif

struct DDSPixelFormat {
    std::uint32_t ByteSize;
    std::uint32_t Flags;
    std::uint32_t FourCC;
    std::uint32_t RgbBitCount;
    std::uint32_t RedBitMask;
    std::uint32_t GreenBitMask;
    std::uint32_t BlueBitMask;
    std::uint32_t AlphaBitMask;
} POMDOG_DETAIL_PACKED;

struct DDSHeader {
    std::uint32_t  ByteSize;
    std::uint32_t  Flags;
    std::uint32_t  PixelHeight;
    std::uint32_t  PixelWidth;
    std::uint32_t  LinearSize;
    std::uint32_t  Depth;
    std::uint32_t  MipMapCount;
    std::uint32_t  Reserved1[11];
    DDSPixelFormat PixelFormat;
    std::uint32_t  Caps;
    std::uint32_t  Caps2;
    std::uint32_t  Caps3;
    std::uint32_t  Caps4;
    std::uint32_t  Reserved2;
} POMDOG_DETAIL_PACKED;

struct DDSHeaderDXT10 {
    std::uint32_t DxgiFormat;        // Note: enum DXGI_FORMAT
    std::uint32_t ResourceDimension; // Note: enum D3D10_RESOURCE_DIMENSION
    std::uint32_t MiscFlag;
    std::uint32_t ArraySize;
    std::uint32_t Reserved;
} POMDOG_DETAIL_PACKED;

#if defined(POMDOG_DETAIL_PACKED)
#    undef POMDOG_DETAIL_PACKED
#endif

#if defined(POMDOG_COMPILER_MSVC)
#pragma pack(pop)
#endif

namespace DirectDrawPixelFormat {

static constexpr std::uint32_t FourCC      = 0x00000004; // DDPF_FOURCC
static constexpr std::uint32_t AlphaPixels = 0x00000001; // DDPF_ALPHAPIXELS
static constexpr std::uint32_t Alpha       = 0x00000002; // DDPF_ALPHA
static constexpr std::uint32_t RGB         = 0x00000040; // DDPF_RGB
static constexpr std::uint32_t Luminance   = 0x00020000; // DDPF_LUMINANCE

} // namespace DirectDrawPixelFormat

SurfaceFormat ToSurfaceFormatFromDDSPixelFormat(const DDSPixelFormat& pixelFormat)
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
        switch (pixelFormat.RgbBitCount) {
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
        if (8 == pixelFormat.RgbBitCount) {
            return SurfaceFormat::A8_UNorm;
        }
    }
    else if (pixelFormat.Flags & DirectDrawPixelFormat::Luminance) {
        switch (pixelFormat.RgbBitCount) {
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

    POMDOG_THROW_EXCEPTION(std::runtime_error,
        "Cannot find the surface format. Undefined or not supported");
}

std::size_t ComputePixelDataByteLength(const DDSHeader& ddsHeader)
{
    auto const bytesPerBlock = ddsHeader.PixelFormat.RgbBitCount;
    auto const levelCount = (ddsHeader.MipMapCount > 0) ? ddsHeader.MipMapCount: 1;

    std::size_t result = 0;
    std::size_t mipMapPixelWidth = ddsHeader.PixelWidth;
    std::size_t mipMapPixelHeight = ddsHeader.PixelHeight;

    for (std::uint32_t mipmapLevel = 0; mipmapLevel < levelCount; ++mipmapLevel) {
        std::size_t const strideBytesPerMipMap = ((mipMapPixelWidth + 3)/4) * ((mipMapPixelHeight + 3)/4) * bytesPerBlock;

        result += strideBytesPerMipMap;

        mipMapPixelWidth = (mipMapPixelWidth >> 1) ? (mipMapPixelWidth >> 1): 1;
        mipMapPixelHeight = (mipMapPixelHeight >> 1) ? (mipMapPixelHeight >> 1): 1;
    }
    return result;
}

} // unnamed namespace

std::shared_ptr<Texture2D> DDSTextureReader::Read(
    const std::shared_ptr<GraphicsDevice>& graphicsDevice,
    const std::uint8_t* data,
    std::size_t byteLength)
{
    POMDOG_ASSERT(graphicsDevice);
    POMDOG_ASSERT(data != nullptr);
    POMDOG_ASSERT(byteLength > 0);

    std::size_t offsetBytes = 0;

    if (!BinaryReader::CanRead<DDSHeader>(byteLength - offsetBytes)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented.");
    }
    auto const ddsHeader = BinaryReader::Read<DDSHeader>(data);
    offsetBytes += sizeof(ddsHeader);

    if (ddsHeader.ByteSize != sizeof(DDSHeader)) {
        POMDOG_THROW_EXCEPTION(std::domain_error, "DDSHeader has invalid format.");
    }
    if (ddsHeader.PixelFormat.ByteSize != sizeof(DDSPixelFormat)) {
        POMDOG_THROW_EXCEPTION(std::domain_error, "DDSHeader has invalid format.");
    }

    bool hasDXT10Header = false;

    if ((ddsHeader.PixelFormat.Flags & DirectDrawPixelFormat::FourCC) &&
        (MakeFourCC('D', 'X', '1', '0') == ddsHeader.PixelFormat.FourCC)) {
        hasDXT10Header = true;
    }

    if (hasDXT10Header) {
        ///@todo Not implemented.
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Sorry, DXT10 header is not supported.");
    }

    std::uint32_t pixelWidth = ddsHeader.PixelWidth;
    std::uint32_t pixelHeight = ddsHeader.PixelHeight;
    SurfaceFormat surfaceFormat = ToSurfaceFormatFromDDSPixelFormat(ddsHeader.PixelFormat);
    bool const generateMipmap = (ddsHeader.MipMapCount > 0);

    if ((byteLength - offsetBytes) < ComputePixelDataByteLength(ddsHeader)) {
        POMDOG_THROW_EXCEPTION(std::domain_error, "DDSHeader has invalid format.");
    }

    auto texture = std::make_shared<Texture2D>(graphicsDevice,
        pixelWidth, pixelHeight, generateMipmap, surfaceFormat);

    if (texture) {
        texture->SetData(data + offsetBytes);
    }

    return texture;
}

} // namespace Detail
} // namespace Pomdog
