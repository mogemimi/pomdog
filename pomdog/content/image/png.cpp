// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/image/png.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/image/image_container.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/scope_guard.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
extern "C" {
#include <png.h>
}
#include <cstring>
#include <limits>
#include <span>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

struct PNGBinaryContext final {
    std::span<const u8> byteData = {};
    std::size_t offset = 0;
};

void readPNGDataCallback(::png_structp png_ptr, ::png_bytep data, ::png_size_t length)
{
    auto context = static_cast<PNGBinaryContext*>(::png_get_io_ptr(png_ptr));

    if ((context->offset + length) <= context->byteData.size()) {
        std::memcpy(data, context->byteData.data() + context->offset, length);
        context->offset += length;
    }
    else {
        ::png_error(png_ptr, "readPNGDataCallback failed.");
    }
}

} // namespace

[[nodiscard]] std::tuple<ImageContainer, std::unique_ptr<Error>>
decodePNG(const u8* data, std::size_t byteLength)
{
    ImageContainer image = {};
    image.mipmapCount = 0;

    auto pngPtr = ::png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (nullptr == pngPtr) {
        return std::make_tuple(std::move(image), errors::make("png_create_read_struct() failed"));
    }

    auto infoPtr = ::png_create_info_struct(pngPtr);

    ScopeGuard scopedDestroyReadStruct([&] {
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
        return std::make_tuple(std::move(image), errors::make("infoPtr is null"));
    }

    PNGBinaryContext context = {};
    context.byteData = std::span<const u8>{data, byteLength};
    context.offset = 0;
    ::png_set_read_fn(pngPtr, &context, readPNGDataCallback);

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

    // NOTE: Get PNG image information
    const auto pixelWidth = ::png_get_image_width(pngPtr, infoPtr);
    const auto pixelHeight = ::png_get_image_height(pngPtr, infoPtr);
    const auto colorType = ::png_get_color_type(pngPtr, infoPtr);
    //const auto bitDepth = ::png_get_bit_depth(pngPtr, infoPtr);

    if (colorType == PNG_COLOR_TYPE_RGB) {
        return std::make_tuple(std::move(image), errors::make("unsupported color type"));
    }
    if (colorType != PNG_COLOR_TYPE_GRAY &&
        colorType != PNG_COLOR_TYPE_GRAY_ALPHA &&
        colorType != PNG_COLOR_TYPE_RGB_ALPHA) {
        return std::make_tuple(std::move(image), errors::make("unsupported color type"));
    }
    if (pixelWidth <= 0) {
        return std::make_tuple(std::move(image), errors::make("invalid image width"));
    }
    if (pixelHeight <= 0) {
        return std::make_tuple(std::move(image), errors::make("invalid image height"));
    }
    if (pixelWidth > static_cast<std::size_t>(std::numeric_limits<i32>::max())) {
        return std::make_tuple(std::move(image), errors::make("too large image width"));
    }
    if (pixelHeight > static_cast<std::size_t>(std::numeric_limits<i32>::max())) {
        return std::make_tuple(std::move(image), errors::make("too large image height"));
    }

    // NOTE: Read PNG image data
    const auto rowBytes = ::png_get_rowbytes(pngPtr, infoPtr);
    std::vector<u8> rowData(rowBytes * pixelHeight * sizeof(png_byte));

    std::vector<::png_bytep> bytePointers(pixelHeight, nullptr);

    for (u32 index = 0; index < pixelHeight; ++index) {
        POMDOG_ASSERT(rowData.size() > rowBytes * index);
        bytePointers[index] = &rowData[rowBytes * index];
    }

    ::png_read_image(pngPtr, bytePointers.data());
    ::png_read_end(pngPtr, nullptr);

    image.width = static_cast<i32>(pixelWidth);
    image.height = static_cast<i32>(pixelHeight);
    image.format = ([](::png_byte colorTypeIn) -> gpu::PixelFormat {
        POMDOG_ASSERT(colorTypeIn != PNG_COLOR_TYPE_RGB);
        switch (colorTypeIn) {
        case PNG_COLOR_TYPE_GRAY:
            return gpu::PixelFormat::R8_UNorm;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            return gpu::PixelFormat::R8G8_UNorm;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            return gpu::PixelFormat::R8G8B8A8_UNorm;
        default:
            POMDOG_ASSERT(colorTypeIn != PNG_COLOR_TYPE_PALETTE);
            break;
        }
        return gpu::PixelFormat::A8_UNorm;
    })(colorType);

    image.rawData = std::move(rowData);
    image.pixelData = std::span<const u8>{image.rawData};

    return std::make_tuple(std::move(image), nullptr);
}

} // namespace pomdog
