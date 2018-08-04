// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/detail/AssetLoaders/AudioClipLoader.hpp"
#include "../Utility/MSWaveAudioLoader.hpp"
#include "Pomdog/Audio/AudioClip.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Content/Utility/MakeFourCC.hpp"
#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Pomdog {
namespace Detail {

std::shared_ptr<AudioClip> AssetLoader<AudioClip>::operator()(
    const AssetLoaderContext& loaderContext, const std::string& assetName)
{
    auto binaryStream = loaderContext.OpenStream(assetName);

    if (!binaryStream.Stream) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Cannot open the wave file: " + assetName);
    }

    if (binaryStream.SizeInBytes < (sizeof(std::uint8_t) * 12)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "The audio file is too small: " + assetName);
    }

    auto & stream = binaryStream.Stream;

    auto signature = BinaryReader::ReadArray<std::uint8_t, 12>(stream);
    if (stream.fail()) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to read the audio file.");
    }

    const auto fourCC = MakeFourCC(signature[0], signature[1], signature[2], signature[3]);

    if (fourCC == MakeFourCC('R', 'I', 'F', 'F')) {
        const auto fccType = MakeFourCC(signature[8], signature[9], signature[10], signature[11]);
        if (fccType == MakeFourCC('W', 'A', 'V', 'E')) {
            // This file format is RIFF waveform audio.
            auto audioClip = MSWaveAudioLoader::Load(std::move(binaryStream));
            return std::move(audioClip);
        }
    }
    else if (fourCC == MakeFourCC('O', 'g', 'g', 'S')) {
        // This file format is Ogg Vorbis.
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Pomdog does not yet supported Ogg Vorbis format.");
    }

    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented.");
}

} // namespace Detail
} // namespace Pomdog
