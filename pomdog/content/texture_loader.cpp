// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/texture_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/image/dds.h"
#include "pomdog/content/image/image_container.h"
#include "pomdog/content/image/png.h"
#include "pomdog/content/image/pnm.h"
#include "pomdog/content/utility/make_fourcc.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <span>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

using detail::makeFourCC;

bool isPNGFormat(const std::uint8_t* data) noexcept
{
    constexpr std::array<std::uint8_t, 8> pngSignature{{0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}};
    return std::equal(pngSignature.begin(), pngSignature.end(), data);
}

bool isDDSFormat(const std::uint8_t* data) noexcept
{
    constexpr auto fourCC = makeFourCC('D', 'D', 'S', ' ');
    static_assert(fourCC == 0x20534444, "The four character code value is 'DDS '");
    return (makeFourCC(data[0], data[1], data[2], data[3]) == fourCC);
}

bool isPNMFormat(const std::uint8_t* data) noexcept
{
    std::string_view view{reinterpret_cast<const char*>(data), 2};
    if ((view != "P1") &&
        (view != "P2") &&
        (view != "P3") &&
        (view != "P4") &&
        (view != "P5") &&
        (view != "P6")) {
        return false;
    }

    const auto c = static_cast<char>(data[2]);
    return (c == ' ') || (c == '\r') || (c == '\n');
}

[[nodiscard]] std::tuple<std::vector<u8>, std::unique_ptr<Error>>
readFileAllBytes(const std::shared_ptr<vfs::File>& file) noexcept
{
    auto [fileInfo, statErr] = file->stat();
    if (statErr != nullptr) {
        return std::make_tuple(std::vector<u8>{}, errors::wrap(std::move(statErr), "failed to get file info"));
    }

    constexpr std::size_t signatureSize = 8;
    if (fileInfo.size < signatureSize) {
        return std::make_tuple(std::vector<u8>{}, errors::make("the texture file is too small"));
    }

    constexpr std::size_t maxFileSize = 1ULL * 1024 * 1024 * 1024; // 1 GB
    if (fileInfo.size > maxFileSize) {
        return std::make_tuple(std::vector<u8>{}, errors::make("the texture file is too large"));
    }

    std::vector<u8> binary(fileInfo.size);
    auto [bytesRead, readErr] = file->read(std::span<u8>{binary.data(), binary.size()});
    if (readErr != nullptr) {
        return std::make_tuple(std::vector<u8>{}, errors::wrap(std::move(readErr), "failed to read the file"));
    }
    binary.resize(bytesRead);
    return std::make_tuple(std::move(binary), nullptr);
}

[[nodiscard]] std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
loadTexture2DFromBytes(
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::vector<u8>& binary) noexcept
{
    constexpr std::size_t signatureSize = 8;
    if (binary.size() < signatureSize) {
        return {nullptr, errors::make("the texture file is too small")};
    }

    POMDOG_ASSERT(graphicsDevice);

    const auto decodeAndCreateTexture = [&](auto&& decodeFn, const char* formatName)
        -> std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>> {
        auto [image, decodeErr] = decodeFn();
        if (decodeErr != nullptr) {
            return {nullptr, errors::wrap(std::move(decodeErr),
                                 std::string("cannot load the ") + formatName + " texture")};
        }

        constexpr bool generateMipmap = false;

        auto texture = std::get<0>(graphicsDevice->createTexture2D(
            image.width,
            image.height,
            generateMipmap,
            image.format));

        POMDOG_ASSERT(!image.pixelData.empty());
        POMDOG_ASSERT(image.pixelData.data() != nullptr);
        texture->setData(image.pixelData.data());

        return {std::move(texture), nullptr};
    };

    if (isPNGFormat(binary.data())) {
        return decodeAndCreateTexture([&]() { return decodePNG(binary.data(), binary.size()); }, "PNG");
    }
    if (isDDSFormat(binary.data())) {
        return decodeAndCreateTexture([&]() { return decodeDDS(binary.data(), binary.size()); }, "DDS");
    }
    if (isPNMFormat(binary.data())) {
        return decodeAndCreateTexture(
            [&]() { return decodePNM(reinterpret_cast<const char*>(binary.data()), binary.size()); }, "PNM");
    }

    return {nullptr, errors::make("this texture file format is not supported")};
}

} // namespace

std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
loadTexture2D(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    std::string_view filePath) noexcept
{
    const std::string path{filePath};

    auto [file, openErr] = vfs::open(fs, path);
    if (openErr != nullptr) {
        return {nullptr, errors::wrap(std::move(openErr), "failed to open file, " + path)};
    }

    auto [binary, readErr] = readFileAllBytes(file);
    if (readErr != nullptr) {
        return {nullptr, errors::wrap(std::move(readErr), "failed to read file, " + path)};
    }

    auto [texture, err] = loadTexture2DFromBytes(graphicsDevice, binary);
    if (err != nullptr) {
        return {nullptr, errors::wrap(std::move(err), "failed to load texture, " + path)};
    }
    return {std::move(texture), nullptr};
}

std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
loadTexture2D(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    u64 mountVolumeHashKey,
    u64 fileHashKey) noexcept
{
    auto [file, openErr] = vfs::open(fs, mountVolumeHashKey, fileHashKey);
    if (openErr != nullptr) {
        return {nullptr, errors::wrap(std::move(openErr), "failed to open texture by hash key")};
    }

    auto [binary, readErr] = readFileAllBytes(file);
    if (readErr != nullptr) {
        return {nullptr, errors::wrap(std::move(readErr), "failed to read texture by hash key, " + std::to_string(fileHashKey))};
    }

    auto [texture, err] = loadTexture2DFromBytes(graphicsDevice, binary);
    if (err != nullptr) {
        return {nullptr, errors::wrap(std::move(err), "failed to load texture by hash key, " + std::to_string(fileHashKey))};
    }
    return {std::move(texture), nullptr};
}

} // namespace pomdog
