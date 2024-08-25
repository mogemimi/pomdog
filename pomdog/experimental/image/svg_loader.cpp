// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image/svg_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/scope_guard.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstring>
#include <fstream>
#include <vector>

#define NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#pragma clang diagnostic ignored "-Wimplicit-float-conversion"
#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#if (__clang_major__ >= 13) && !defined(__APPLE_CC__)
#pragma clang diagnostic ignored "-Wreserved-identifier"
#endif
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wdouble-promotion"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4244 4456 4702)
#endif
#include <nanosvg/src/nanosvg.h>
#include <nanosvg/src/nanosvgrast.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::SVG {
namespace {

std::tuple<ImageBuffer, std::unique_ptr<Error>>
DecodeSVG(std::uint8_t* data, std::size_t size, int canvasWidth, int canvasHeight)
{
    ImageBuffer imageBuffer;

    if ((canvasWidth <= 0) || (canvasHeight <= 0)) {
        auto err = errors::make("invalid width or height");
        return std::make_tuple(std::move(imageBuffer), std::move(err));
    }

    if (size <= 0) {
        auto err = errors::make("invalid size");
        return std::make_tuple(std::move(imageBuffer), std::move(err));
    }

    auto image = nsvgParse(reinterpret_cast<char*>(data), "px", 96);
    if (image == nullptr) {
        auto err = errors::make("failed to parse svg");
        return std::make_tuple(std::move(imageBuffer), std::move(err));
    }

    [[maybe_unused]] detail::ScopeGuard defer([&] { nsvgDelete(image); });

    auto rasterizer = nsvgCreateRasterizer();
    if (rasterizer == nullptr) {
        auto err = errors::make("could not initialize SVG rasterizer");
        return std::make_tuple(std::move(imageBuffer), std::move(err));
    }

    [[maybe_unused]] detail::ScopeGuard defer2([&] { nsvgDeleteRasterizer(rasterizer); });

    if ((image->width <= 0.0f) || (image->height <= 0.0f)) {
        auto err = errors::make("invalid svg format");
        return std::make_tuple(std::move(imageBuffer), std::move(err));
    }

    const float scale = std::max(
        std::min(
            static_cast<float>(canvasWidth) / image->width,
            static_cast<float>(canvasHeight) / image->height),
        0.000001f);

    imageBuffer.RawData.resize(canvasWidth * canvasHeight * 4);
    imageBuffer.PixelData = imageBuffer.RawData.data();
    imageBuffer.ByteLength = imageBuffer.RawData.size();
    imageBuffer.Format = PixelFormat::R8G8B8A8_UNorm;
    imageBuffer.Width = canvasWidth;
    imageBuffer.Height = canvasHeight;

    nsvgRasterize(
        rasterizer,
        image,
        0.0f,
        0.0f,
        scale,
        reinterpret_cast<unsigned char*>(imageBuffer.RawData.data()),
        canvasWidth,
        canvasHeight,
        canvasWidth * 4);

    return std::make_tuple(std::move(imageBuffer), nullptr);
}

} // namespace

std::tuple<ImageBuffer, std::unique_ptr<Error>>
Decode(
    const std::uint8_t* data,
    std::size_t size,
    int width,
    int height)
{
    POMDOG_ASSERT(data != nullptr);
    POMDOG_ASSERT(size > 0);
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    std::vector<std::uint8_t> buffer;
    buffer.resize(size);
    std::memcpy(buffer.data(), data, buffer.size());

    return DecodeSVG(buffer.data(), buffer.size(), width, height);
}

std::tuple<ImageBuffer, std::unique_ptr<Error>>
DecodeFile(
    const std::string& filePath,
    int width,
    int height)
{
    std::ifstream stream{filePath, std::ifstream::binary};

    if (!stream) {
        auto err = errors::make("cannot open the file, " + filePath);
        return std::make_tuple(ImageBuffer{}, std::move(err));
    }

    auto [byteLength, sizeErr] = FileSystem::getFileSize(filePath);
    if (sizeErr != nullptr) {
        auto err = errors::wrap(std::move(sizeErr), "failed to get file size, " + filePath);
        return std::make_tuple(ImageBuffer{}, std::move(err));
    }

    POMDOG_ASSERT(stream);

    std::vector<std::uint8_t> binary;
    binary.resize(byteLength);
    stream.read(reinterpret_cast<char*>(binary.data()), binary.size());
    if (!stream) {
        auto err = errors::make("failed to read the file " + filePath);
        return std::make_tuple(ImageBuffer{}, std::move(err));
    }

    return DecodeSVG(binary.data(), binary.size(), width, height);
}

std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
LoadTexture(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::string& filePath,
    int width,
    int height)
{
    POMDOG_ASSERT(graphicsDevice != nullptr);
    POMDOG_ASSERT(!filePath.empty());
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    auto [image, decodeErr] = DecodeFile(filePath, width, height);
    if (decodeErr != nullptr) {
        auto err = errors::wrap(std::move(decodeErr), "failed to decode SVG file, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    // FIXME: Add support multi-level texture (mipmap)
    constexpr bool generateMipmap = false;

    auto texture = std::get<0>(graphicsDevice->createTexture2D(
        image.Width,
        image.Height,
        generateMipmap,
        image.Format));

    POMDOG_ASSERT(image.PixelData != nullptr);
    POMDOG_ASSERT(image.ByteLength > 0);

    texture->setData(image.PixelData);

    return std::make_tuple(std::move(texture), nullptr);
}

} // namespace pomdog::SVG
