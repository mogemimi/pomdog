// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/AssetLoaders/AudioClipLoader.hpp"
#include "Pomdog/Audio/AudioClip.hpp"
#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Content/Audio/Vorbis.hpp"
#include "Pomdog/Content/Audio/WAV.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Content/Utility/MakeFourCC.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/FileSystem.hpp"
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Pomdog::Detail {

std::tuple<std::shared_ptr<AudioClip>, std::shared_ptr<Error>>
AssetLoader<AudioClip>::operator()([[maybe_unused]] AssetManager& assets, const std::string& filePath)
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

    if (byteLength < (sizeof(std::uint8_t) * 12)) {
        auto err = Errors::New("The audio file is too small " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    auto signature = BinaryReader::ReadArray<std::uint8_t, 12>(stream);
    if (stream.fail()) {
        auto err = Errors::New("failed to read signature in the file " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    stream.clear();
    stream.seekg(0, std::ios_base::beg);

    const auto fourCC = MakeFourCC(signature[0], signature[1], signature[2], signature[3]);

    if (fourCC == MakeFourCC('R', 'I', 'F', 'F')) {
        const auto fccType = MakeFourCC(signature[8], signature[9], signature[10], signature[11]);
        if (fccType == MakeFourCC('W', 'A', 'V', 'E')) {
            // NOTE: This file format is RIFF waveform audio.
            auto [audioClip, loadErr] = WAV::Load(std::move(stream), byteLength);

            if (loadErr != nullptr) {
                auto err = Errors::Wrap(std::move(loadErr), "Cannot load the wave file " + filePath);
                return std::make_tuple(nullptr, std::move(err));
            }
            return std::make_tuple(std::move(audioClip), nullptr);
        }
    }
    else if (fourCC == MakeFourCC('O', 'g', 'g', 'S')) {
        // NOTE: The file format is Ogg Vorbis.
        stream.close();

        auto [audioClip, loadErr] = Vorbis::Load(filePath);
        if (loadErr != nullptr) {
            auto err = Errors::Wrap(std::move(loadErr), "Cannot load the ogg/vorbis file " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }
        return std::make_tuple(std::move(audioClip), nullptr);
    }

    auto err = Errors::New("This audio file format is not supported " + filePath);
    return std::make_tuple(nullptr, std::move(err));
}

} // namespace Pomdog::Detail
