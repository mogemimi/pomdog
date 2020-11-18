// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/AssetLoaders/Texture2DLoader.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Content/Image/DDS.hpp"
#include "Pomdog/Content/Image/PNG.hpp"
#include "Pomdog/Content/Image/PNM.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Content/Utility/MakeFourCC.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/SurfaceFormat.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/FileSystem.hpp"
#include <algorithm>
#include <array>
#include <fstream>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Pomdog::Detail {
namespace {

bool IsPNGFormat(const std::array<std::uint8_t, 8>& signature) noexcept
{
    std::array<std::uint8_t, 8> const pngSignature {{
        0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}};

    return std::equal(std::begin(signature), std::end(signature), std::begin(pngSignature));
}

bool IsDDSFormat(const std::array<std::uint8_t, 8>& signature) noexcept
{
    constexpr auto fourCC = MakeFourCC('D', 'D', 'S', ' ');
    static_assert(fourCC == 0x20534444, "The four character code value is 'DDS '");
    return (MakeFourCC(signature[0], signature[1], signature[2], signature[3]) == fourCC);
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

    auto s = reinterpret_cast<const char*>(signature.data() + 2);
    return (*s == ' ') || (*s == '\n');
}

} // namespace

std::tuple<std::shared_ptr<Texture2D>, std::shared_ptr<Error>>
AssetLoader<Texture2D>::operator()(AssetManager& assets, const std::string& filePath)
{
    std::ifstream stream{filePath, std::ifstream::binary};

    if (!stream) {
        auto err = Errors::New("cannot open the file, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    auto [byteLength, sizeErr] = FileSystem::GetFileSize(filePath);
    if (sizeErr != nullptr) {
        auto err = Errors::Wrap(std::move(sizeErr), "failed to get file size, " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    POMDOG_ASSERT(stream);

    constexpr std::size_t signatureArraySize = 8;
    if (byteLength < (sizeof(std::uint8_t) * signatureArraySize)) {
        auto err = Errors::New("The texture file is too small " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    auto signature = BinaryReader::ReadArray<std::uint8_t, signatureArraySize>(stream);
    if (stream.fail()) {
        auto err = Errors::New("failed to read signature in the file " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    stream.clear();
    stream.seekg(0, stream.beg);

    auto graphicsDevice = assets.GetGraphicsDevice();
    POMDOG_ASSERT(graphicsDevice);

    if (IsPNGFormat(signature)) {
        std::vector<std::uint8_t> binary;
        binary.resize(byteLength);
        stream.read(reinterpret_cast<char*>(binary.data()), binary.size());
        if (!stream) {
            auto err = Errors::New("failed to read the file " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }

        auto [image, decodeErr] = PNG::Decode(binary.data(), binary.size());
        if (decodeErr != nullptr) {
            auto err = Errors::Wrap(std::move(decodeErr), "cannot load the PNG texture " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }

        constexpr bool generateMipmap = false;

        auto texture = std::get<0>(graphicsDevice->CreateTexture2D(
            image.Width,
            image.Height,
            generateMipmap,
            image.Format));

        POMDOG_ASSERT(!image.RawData.empty());
        POMDOG_ASSERT(image.PixelData != nullptr);
        POMDOG_ASSERT(image.ByteLength > 0);

        texture->SetData(image.PixelData);

        return std::make_tuple(std::move(texture), nullptr);
    }
    else if (IsDDSFormat(signature)) {
        std::vector<std::uint8_t> binary;
        binary.resize(byteLength);
        stream.read(reinterpret_cast<char*>(binary.data()), binary.size());
        if (!stream) {
            auto err = Errors::New("failed to read the file " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }

        auto [image, decodeErr] = DDS::Decode(binary.data(), binary.size());
        if (decodeErr != nullptr) {
            auto err = Errors::Wrap(std::move(decodeErr), "cannot load the DDS texture " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }

        // FIXME: Add support multi-level texture (mipmap)
        constexpr bool generateMipmap = false;

        auto texture = std::get<0>(graphicsDevice->CreateTexture2D(
            image.Width,
            image.Height,
            generateMipmap,
            image.Format));

        POMDOG_ASSERT(image.PixelData != nullptr);
        POMDOG_ASSERT(image.ByteLength > 0);

        texture->SetData(image.PixelData);

        return std::make_tuple(std::move(texture), nullptr);
    }
    else if (IsPNMFormat(signature)) {
        std::vector<std::uint8_t> binary;
        binary.resize(byteLength);
        stream.read(reinterpret_cast<char*>(binary.data()), binary.size());
        if (!stream) {
            auto err = Errors::New("failed to read the file " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }

        auto [image, decodeErr] = PNM::Decode(reinterpret_cast<const char*>(binary.data()), binary.size());
        if (decodeErr != nullptr) {
            auto err = Errors::Wrap(std::move(decodeErr), "cannot load the PNM texture " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }

        constexpr bool generateMipmap = false;

        auto texture = std::get<0>(graphicsDevice->CreateTexture2D(
            image.Width,
            image.Height,
            generateMipmap,
            image.Format));

        POMDOG_ASSERT(image.PixelData != nullptr);
        POMDOG_ASSERT(image.ByteLength > 0);
        texture->SetData(image.PixelData);

        return std::make_tuple(std::move(texture), nullptr);
    }

    auto err = Errors::New("This texture file format is not supported " + filePath);
    return std::make_tuple(nullptr, std::move(err));
}

} // namespace Pomdog::Detail
