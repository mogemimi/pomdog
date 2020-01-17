// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/Image/PNG.hpp"
#include "../../Utility/ScopeGuard.hpp"
#include "Pomdog/Utility/Assert.hpp"
extern "C" {
#include <png.h>
}
#include <cstring>
#include <vector>

namespace Pomdog::PNG {
namespace {

struct PNGBinaryContext final {
    const std::uint8_t* Data;
    std::size_t ByteLength;
    std::size_t Offset;
};

void ReadPNGDataCallback(::png_structp png_ptr, ::png_bytep data, ::png_size_t length)
{
    auto context = static_cast<PNGBinaryContext*>(::png_get_io_ptr(png_ptr));

    if ((context->Offset + length) <= context->ByteLength) {
        std::memcpy(data, context->Data + context->Offset, length);
        context->Offset += length;
    }
    else {
        ::png_error(png_ptr, "ReadPngCallback failed.");
    }
}

} // namespace

std::tuple<ImageBuffer, std::shared_ptr<Error>>
Decode(const std::uint8_t* data, std::size_t byteLength)
{
    ImageBuffer image;
    image.PixelData = nullptr;
    image.ByteLength = 0;
    image.MipmapCount = 0;

    auto pngPtr = ::png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (nullptr == pngPtr) {
        return std::make_tuple(std::move(image), Errors::New("png_create_read_struct() failed"));
    }

    auto infoPtr = ::png_create_info_struct(pngPtr);

    Detail::ScopeGuard scopedDestroyReadStruct([&] {
        if (nullptr != pngPtr) {
            if (nullptr != infoPtr) {
                ::png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
            }
            else {
                ::png_destroy_read_struct(&pngPtr, nullptr, nullptr);
            }
        }
    });

    if (nullptr == infoPtr) {
        return std::make_tuple(std::move(image), Errors::New("infoPtr is null"));
    }

    PNGBinaryContext context;
    context.Data = data;
    context.ByteLength = byteLength;
    context.Offset = 0;
    ::png_set_read_fn(pngPtr, &context, ReadPNGDataCallback);

    // NOTE: If we have already read some of the signature.
    constexpr auto pngSignatureByteLength = 0;
    ::png_set_sig_bytes(pngPtr, pngSignatureByteLength);

    // NOTE: Read PNG header.
    ::png_read_info(pngPtr, infoPtr);
    auto const originalColorType = ::png_get_color_type(pngPtr, infoPtr);
    auto const originalBitDepth = ::png_get_bit_depth(pngPtr, infoPtr);

    // NOTE: Settings
    if (originalColorType == PNG_COLOR_TYPE_PALETTE) {
        // NOTE: PALETTE => RGB (24 bit)
        ::png_set_palette_to_rgb(pngPtr);
    }

    if (originalColorType == PNG_COLOR_TYPE_GRAY && originalBitDepth < 8) {
        ::png_set_expand_gray_1_2_4_to_8(pngPtr);
    }
    else if (originalBitDepth < 8) {
        ::png_set_packing(pngPtr);
    }
    else if (originalBitDepth == 16) {
        ::png_set_strip_16(pngPtr);
    }

    if (::png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS)) {
        ::png_set_tRNS_to_alpha(pngPtr);
    }
    else {
        switch (originalColorType) {
        case PNG_COLOR_TYPE_PALETTE:
        case PNG_COLOR_TYPE_RGB:
            // NOTE: RGB (24 bit) => RGBA (32 bit)
            ::png_set_add_alpha(pngPtr, 0xFF, PNG_FILLER_AFTER);
            break;
        default:
            break;
        }
    }

    ::png_read_update_info(pngPtr, infoPtr);

    // Texture2D Info
    auto const pixelWidth = ::png_get_image_width(pngPtr, infoPtr);
    auto const pixelHeight = ::png_get_image_height(pngPtr, infoPtr);
    auto const colorType = ::png_get_color_type(pngPtr, infoPtr);
    //auto const bitDepth = ::png_get_bit_depth(pngPtr, infoPtr);

    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);

    // NOTE: Read PNG image data
    auto const rowBytes = ::png_get_rowbytes(pngPtr, infoPtr);
    std::vector<std::uint8_t> rowData(rowBytes * pixelHeight * sizeof(png_byte));

    std::vector<::png_bytep> bytePointers(pixelHeight, nullptr);

    for (std::uint32_t index = 0; index < pixelHeight; ++index) {
        POMDOG_ASSERT(rowData.size() > rowBytes * index);
        bytePointers[index] = &rowData[rowBytes * index];
    }

    ::png_read_image(pngPtr, bytePointers.data());
    ::png_read_end(pngPtr, nullptr);

    image.Width = pixelWidth;
    image.Height = pixelHeight;
    image.Format = ([](::png_byte colorTypeIn) -> SurfaceFormat {
        POMDOG_ASSERT(colorTypeIn != PNG_COLOR_TYPE_RGB);
        switch (colorTypeIn) {
        case PNG_COLOR_TYPE_GRAY:
            return SurfaceFormat::R8_UNorm;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            return SurfaceFormat::R8G8_UNorm;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            return SurfaceFormat::R8G8B8A8_UNorm;
        default:
            POMDOG_ASSERT(colorTypeIn != PNG_COLOR_TYPE_PALETTE);
            break;
        }
        return SurfaceFormat::A8_UNorm;
    })(colorType);

    image.RawData = std::move(rowData);
    image.PixelData = image.RawData.data();
    image.ByteLength = image.RawData.size();

    return std::make_tuple(std::move(image), nullptr);
}

} // namespace Pomdog::PNG
