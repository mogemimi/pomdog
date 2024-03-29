// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/asset_loaders/texture2d_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/content/image/dds.h"
#include "pomdog/content/image/png.h"
#include "pomdog/content/image/pnm.h"
#include "pomdog/content/utility/binary_reader.h"
#include "pomdog/content/utility/make_fourcc.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <fstream>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
namespace {

bool IsPNGFormat(const std::array<std::uint8_t, 8>& signature) noexcept
{
    std::array<std::uint8_t, 8> const pngSignature{{0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}};

    return std::equal(std::begin(signature), std::end(signature), std::begin(pngSignature));
}

bool IsDDSFormat(const std::array<std::uint8_t, 8>& signature) noexcept
{
    constexpr auto fourCC = makeFourCC('D', 'D', 'S', ' ');
    static_assert(fourCC == 0x20534444, "The four character code value is 'DDS '");
    return (makeFourCC(signature[0], signature[1], signature[2], signature[3]) == fourCC);
}

bool IsPNMFormat(const std::array<std::uint8_t, 8>& signature) noexcept
{
    std::string_view view{reinterpret_cast<const char*>(signature.data()), 2};
    if ((view != "P1") &&
        (view != "P2") &&
        (view != "P3") &&
        (view != "P4") &&
        (view != "P5") &&
        (view != "P6")) {
        return false;
    }

    const auto c = static_cast<char>(signature[2]);
    return (c == ' ') || (c == '\r') || (c == '\n');
}

} // namespace

std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
AssetLoader<gpu::Texture2D>::operator()(AssetManager& assets, const std::string& filePath)
{
    std::ifstream stream{filePath, std::ifstream::binary};

    if (!stream) {
        auto err = errors::make("cannot open the file, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    auto [byteLength, sizeErr] = FileSystem::getFileSize(filePath);
    if (sizeErr != nullptr) {
        auto err = errors::wrap(std::move(sizeErr), "failed to get file size, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    POMDOG_ASSERT(stream);

    constexpr std::size_t signatureArraySize = 8;
    if (byteLength < (sizeof(std::uint8_t) * signatureArraySize)) {
        auto err = errors::make("The texture file is too small " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    auto signature = BinaryReader::readArray<std::uint8_t, signatureArraySize>(stream);
    if (stream.fail()) {
        auto err = errors::make("failed to read signature in the file " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    stream.clear();
    stream.seekg(0, stream.beg);

    auto graphicsDevice = assets.getGraphicsDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (IsPNGFormat(signature)) {
        std::vector<std::uint8_t> binary;
        binary.resize(byteLength);
        stream.read(reinterpret_cast<char*>(binary.data()), static_cast<int>(binary.size()));
        if (!stream) {
            auto err = errors::make("failed to read the file " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }

        auto [image, decodeErr] = PNG::decode(binary.data(), binary.size());
        if (decodeErr != nullptr) {
            auto err = errors::wrap(std::move(decodeErr), "cannot load the PNG texture " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }

        constexpr bool generateMipmap = false;

        auto texture = std::get<0>(graphicsDevice->createTexture2D(
            image.Width,
            image.Height,
            generateMipmap,
            image.Format));

        POMDOG_ASSERT(!image.RawData.empty());
        POMDOG_ASSERT(image.PixelData != nullptr);
        POMDOG_ASSERT(image.ByteLength > 0);

        texture->setData(image.PixelData);

        return std::make_tuple(std::move(texture), nullptr);
    }
    else if (IsDDSFormat(signature)) {
        std::vector<std::uint8_t> binary;
        binary.resize(byteLength);
        stream.read(reinterpret_cast<char*>(binary.data()), static_cast<int>(binary.size()));
        if (!stream) {
            auto err = errors::make("failed to read the file " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }

        auto [image, decodeErr] = DDS::decode(binary.data(), binary.size());
        if (decodeErr != nullptr) {
            auto err = errors::wrap(std::move(decodeErr), "cannot load the DDS texture " + filePath);
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
    else if (IsPNMFormat(signature)) {
        std::vector<std::uint8_t> binary;
        binary.resize(byteLength);
        stream.read(reinterpret_cast<char*>(binary.data()), static_cast<int>(binary.size()));
        if (!stream) {
            auto err = errors::make("failed to read the file " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }

        auto [image, decodeErr] = PNM::decode(reinterpret_cast<const char*>(binary.data()), binary.size());
        if (decodeErr != nullptr) {
            auto err = errors::wrap(std::move(decodeErr), "cannot load the PNM texture " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }

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

    auto err = errors::make("This texture file format is not supported " + filePath);
    return std::make_tuple(nullptr, std::move(err));
}

} // namespace pomdog::detail
