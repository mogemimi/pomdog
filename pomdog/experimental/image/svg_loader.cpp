// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image/svg_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/image/image_container.h"
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

POMDOG_CLANG_SUPPRESS_WARNING_PUSH
POMDOG_CLANG_SUPPRESS_WARNING("-Wcast-align")
POMDOG_CLANG_SUPPRESS_WARNING("-Wcast-qual")
POMDOG_CLANG_SUPPRESS_WARNING("-Wdouble-promotion")
POMDOG_CLANG_SUPPRESS_WARNING("-Wfloat-conversion")
POMDOG_CLANG_SUPPRESS_WARNING("-Wimplicit-float-conversion")
POMDOG_CLANG_SUPPRESS_WARNING("-Wimplicit-int-conversion")
POMDOG_CLANG_SUPPRESS_WARNING("-Wmissing-prototypes")
POMDOG_CLANG_SUPPRESS_WARNING("-Wmissing-variable-declarations")
POMDOG_CLANG_SUPPRESS_WARNING("-Wold-style-cast")
POMDOG_CLANG_SUPPRESS_WARNING("-Wreserved-id-macro")
POMDOG_CLANG_SUPPRESS_WARNING("-Wshadow")
POMDOG_CLANG_SUPPRESS_WARNING("-Wzero-as-null-pointer-constant")

#if defined(__clang__)
#if (__clang_major__ >= 13) && !defined(__APPLE_CC__)
POMDOG_CLANG_SUPPRESS_WARNING("-Wreserved-identifier")
#endif
#endif

POMDOG_GCC_SUPPRESS_WARNING_PUSH
POMDOG_GCC_SUPPRESS_WARNING("-Wcast-qual")
POMDOG_GCC_SUPPRESS_WARNING("-Wdouble-promotion")
POMDOG_GCC_SUPPRESS_WARNING("-Wmaybe-uninitialized")
POMDOG_GCC_SUPPRESS_WARNING("-Wold-style-cast")
POMDOG_GCC_SUPPRESS_WARNING("-Wshadow")
POMDOG_GCC_SUPPRESS_WARNING("-Wzero-as-null-pointer-constant")

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4244)
POMDOG_MSVC_SUPPRESS_WARNING(4456)
POMDOG_MSVC_SUPPRESS_WARNING(4702)

#include <nanosvg/src/nanosvg.h>
#include <nanosvg/src/nanosvgrast.h>

POMDOG_MSVC_SUPPRESS_WARNING_POP
POMDOG_GCC_SUPPRESS_WARNING_POP
POMDOG_CLANG_SUPPRESS_WARNING_POP

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

std::tuple<ImageContainer, std::unique_ptr<Error>>
rasterizeSVG(std::span<u8> svgData, int canvasWidth, int canvasHeight)
{
    if ((canvasWidth <= 0) || (canvasHeight <= 0)) {
        auto err = errors::make("invalid width or height");
        return std::make_tuple(ImageContainer{}, std::move(err));
    }

    if (svgData.size() <= 0) {
        auto err = errors::make("invalid size");
        return std::make_tuple(ImageContainer{}, std::move(err));
    }

    auto image = nsvgParse(reinterpret_cast<char*>(svgData.data()), "px", 96.0f);
    if (image == nullptr) {
        auto err = errors::make("failed to parse svg");
        return std::make_tuple(ImageContainer{}, std::move(err));
    }

    [[maybe_unused]] ScopeGuard defer([&] { nsvgDelete(image); });

    if ((image->width <= 0.0f) || (image->height <= 0.0f)) {
        auto err = errors::make("invalid svg format");
        return std::make_tuple(ImageContainer{}, std::move(err));
    }

    auto rasterizer = nsvgCreateRasterizer();
    if (rasterizer == nullptr) {
        auto err = errors::make("could not initialize svg rasterizer");
        return std::make_tuple(ImageContainer{}, std::move(err));
    }

    [[maybe_unused]] ScopeGuard defer2([&] { nsvgDeleteRasterizer(rasterizer); });

    const float scale = std::max(
        std::min(
            static_cast<float>(canvasWidth) / image->width,
            static_cast<float>(canvasHeight) / image->height),
        0.000001f);

    const int stride = canvasWidth * 4;

    ImageContainer imageContainer = {};
    imageContainer.rawData.resize(static_cast<std::size_t>(stride * canvasHeight), 0);
    imageContainer.pixelData = std::span<const u8>{imageContainer.rawData};
    imageContainer.format = PixelFormat::R8G8B8A8_UNorm;
    imageContainer.width = canvasWidth;
    imageContainer.height = canvasHeight;

    nsvgRasterize(
        rasterizer,
        image,
        0.0f,
        0.0f,
        scale,
        reinterpret_cast<unsigned char*>(imageContainer.rawData.data()),
        canvasWidth,
        canvasHeight,
        stride);

    return std::make_tuple(std::move(imageContainer), nullptr);
}

} // namespace

std::tuple<ImageContainer, std::unique_ptr<Error>>
decodeSVG(
    std::span<const u8> svgData,
    int width,
    int height)
{
    POMDOG_ASSERT(svgData.data() != nullptr);
    POMDOG_ASSERT(svgData.size() > 0);
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    if (svgData.data() == nullptr || svgData.size() == 0) {
        auto err = errors::make("invalid svg data");
        return std::make_tuple(ImageContainer{}, std::move(err));
    }

    // NOTE: nanosvg modifies the input data, so we need to make a copy of it.
    std::vector<u8> svgDataCopy = {};
    svgDataCopy.reserve(svgData.size() + 1);
    svgDataCopy.resize(svgData.size());
    std::memcpy(svgDataCopy.data(), svgData.data(), svgData.size());

    // NOTE: nanosvg requires null-terminated string input.
    svgDataCopy.push_back(0);

    return rasterizeSVG(svgDataCopy, width, height);
}

std::tuple<ImageContainer, std::unique_ptr<Error>>
decodeSVGFromFile(
    const std::string& filePath,
    int width,
    int height)
{
    if (filePath.empty()) {
        auto err = errors::make("file path is empty");
        return std::make_tuple(ImageContainer{}, std::move(err));
    }

    std::ifstream stream{filePath, std::ifstream::binary};

    if (!stream) {
        auto err = errors::make("cannot open the file, " + filePath);
        return std::make_tuple(ImageContainer{}, std::move(err));
    }

    auto [byteLength, sizeErr] = FileSystem::getFileSize(filePath);
    if (sizeErr != nullptr) {
        auto err = errors::wrap(std::move(sizeErr), "failed to get file size, " + filePath);
        return std::make_tuple(ImageContainer{}, std::move(err));
    }

    POMDOG_ASSERT(stream);

    std::vector<u8> binary = {};
    binary.reserve(byteLength + 1);
    binary.resize(byteLength, 0);

    stream.read(reinterpret_cast<char*>(binary.data()), binary.size());
    if (!stream) {
        auto err = errors::make("failed to read the file " + filePath);
        return std::make_tuple(ImageContainer{}, std::move(err));
    }

    // NOTE: nanosvg requires null-terminated string input.
    binary.push_back(0);

    return rasterizeSVG(binary, width, height);
}

std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
loadTextureFromSVGFile(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::string& filePath,
    int width,
    int height)
{
    POMDOG_ASSERT(graphicsDevice != nullptr);
    POMDOG_ASSERT(!filePath.empty());
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    auto [image, decodeErr] = decodeSVGFromFile(filePath, width, height);
    if (decodeErr != nullptr) {
        auto err = errors::wrap(std::move(decodeErr), "failed to decode SVG file, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    // FIXME: Add support multi-level texture (mipmap)
    constexpr bool generateMipmap = false;

    auto texture = std::get<0>(graphicsDevice->createTexture2D(
        image.width,
        image.height,
        generateMipmap,
        image.format));

    POMDOG_ASSERT(!image.pixelData.empty());
    POMDOG_ASSERT(image.pixelData.data() != nullptr);

    texture->setData(image.pixelData.data());

    return std::make_tuple(std::move(texture), nullptr);
}

} // namespace pomdog
