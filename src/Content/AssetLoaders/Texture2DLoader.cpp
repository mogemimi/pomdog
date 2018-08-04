// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/detail/AssetLoaders/Texture2DLoader.hpp"
#include "../Utility/DDSTextureReader.hpp"
#include "../Utility/PNGTextureReader.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Content/Utility/MakeFourCC.hpp"
#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <algorithm>
#include <array>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Pomdog {
namespace Detail {
namespace {

bool IsPNGFormat(const std::array<std::uint8_t, 8>& signature)
{
    std::array<std::uint8_t, 8> const pngSignature {{
        0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}};

    return std::equal(std::begin(signature), std::end(signature),
        std::begin(pngSignature));
}

bool IsDDSFormat(const std::array<std::uint8_t, 8>& signature)
{
    constexpr auto fourCC = MakeFourCC('D', 'D', 'S', ' ');
    static_assert(fourCC == 0x20534444, "The four character code value is 'DDS '");
    return (MakeFourCC(signature[0], signature[1], signature[2], signature[3]) == fourCC);
}

} // unnamed namespace

std::shared_ptr<Texture2D> AssetLoader<Texture2D>::operator()(
    const AssetLoaderContext& loaderContext, const std::string& assetName)
{
    auto binaryFile = loaderContext.OpenStream(assetName);
    auto & stream = binaryFile.Stream;

    if (stream.fail()) {
        POMDOG_THROW_EXCEPTION(std::invalid_argument, "Cannot open file: " + assetName);
    }

    if (binaryFile.SizeInBytes < (sizeof(std::uint8_t) * 8)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented.");
    }

    auto signature = BinaryReader::ReadArray<std::uint8_t, 8>(stream);
    if (stream.fail()) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented.");
    }

    if (IsPNGFormat(signature)) {
        constexpr auto SignatureLength = sizeof(std::uint8_t) * 8;

        POMDOG_ASSERT(stream.tellg() == std::streampos(SignatureLength));

        std::vector<std::uint8_t> binaryWithoutSignature(binaryFile.SizeInBytes - SignatureLength);
        stream.read(reinterpret_cast<char*>(binaryWithoutSignature.data()), binaryWithoutSignature.size());

        auto graphicsDevice = loaderContext.GraphicsDevice.lock();
        POMDOG_ASSERT(graphicsDevice);

        return PNGTextureReader::Read(graphicsDevice, binaryWithoutSignature.data(), binaryWithoutSignature.size());
    }
    else if (IsDDSFormat(signature)) {
        constexpr auto FourCCLength = sizeof(std::uint8_t) * 4;

        stream.seekg(FourCCLength, stream.beg);

        std::vector<std::uint8_t> binaryWithoutFourCC(binaryFile.SizeInBytes - FourCCLength);
        stream.read(reinterpret_cast<char*>(binaryWithoutFourCC.data()), binaryWithoutFourCC.size());

        auto graphicsDevice = loaderContext.GraphicsDevice.lock();
        POMDOG_ASSERT(graphicsDevice);

        return DDSTextureReader::Read(graphicsDevice, binaryWithoutFourCC.data(), binaryWithoutFourCC.size());
    }

    POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid/unsupported texture format.");
}

} // namespace Detail
} // namespace Pomdog
