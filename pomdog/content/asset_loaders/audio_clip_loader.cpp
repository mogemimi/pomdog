// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/asset_loaders/audio_clip_loader.h"
#include "pomdog/audio/audio_clip.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/asset_manager.h"
#include "pomdog/content/audio/vorbis.h"
#include "pomdog/content/audio/wav.h"
#include "pomdog/content/utility/binary_reader.h"
#include "pomdog/content/utility/make_fourcc.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
AssetLoader<AudioClip>::operator()(AssetManager& assets, const std::string& filePath)
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

    if (byteLength < (sizeof(std::uint8_t) * 12)) {
        auto err = errors::make("The audio file is too small " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    auto signature = BinaryReader::readArray<std::uint8_t, 12>(stream);
    if (stream.fail()) {
        auto err = errors::make("failed to read signature in the file " + filePath);
        return std::make_tuple(nullptr, std::move(err));
    }

    stream.clear();
    stream.seekg(0, std::ios_base::beg);

    const auto fourCC = makeFourCC(signature[0], signature[1], signature[2], signature[3]);

    if (fourCC == makeFourCC('R', 'I', 'F', 'F')) {
        const auto fccType = makeFourCC(signature[8], signature[9], signature[10], signature[11]);
        if (fccType == makeFourCC('W', 'A', 'V', 'E')) {
            // NOTE: This file format is RIFF waveform audio.
            auto [audioClip, loadErr] = WAV::load(assets.getAudioEngine(), std::move(stream), byteLength);

            if (loadErr != nullptr) {
                auto err = errors::wrap(std::move(loadErr), "Cannot load the wave file " + filePath);
                return std::make_tuple(nullptr, std::move(err));
            }
            return std::make_tuple(std::move(audioClip), nullptr);
        }
    }
    else if (fourCC == makeFourCC('O', 'g', 'g', 'S')) {
        // NOTE: The file format is Ogg Vorbis.
        stream.close();

        auto [audioClip, loadErr] = Vorbis::load(assets.getAudioEngine(), filePath);
        if (loadErr != nullptr) {
            auto err = errors::wrap(std::move(loadErr), "Cannot load the ogg/vorbis file " + filePath);
            return std::make_tuple(nullptr, std::move(err));
        }
        return std::make_tuple(std::move(audioClip), nullptr);
    }

    auto err = errors::make("This audio file format is not supported " + filePath);
    return std::make_tuple(nullptr, std::move(err));
}

} // namespace pomdog::detail
