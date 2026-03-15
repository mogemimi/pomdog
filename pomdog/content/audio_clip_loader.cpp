// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/audio_clip_loader.h"
#include "pomdog/audio/audio_clip.h"
#include "pomdog/audio/audio_engine.h"
#include "pomdog/audio/details/audio_clip_file.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/audio/audio_container.h"
#include "pomdog/content/audio/vorbis.h"
#include "pomdog/content/audio/wav.h"
#include "pomdog/content/utility/binary_reader.h"
#include "pomdog/content/utility/make_fourcc.h"
#include "pomdog/memory/memset_span.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

using detail::makeFourCC;
namespace BinaryReader = detail::BinaryReader;

[[nodiscard]] bool
isWAVFormat(std::span<const u8> file) noexcept
{
    if (file.size() < 12) {
        return false;
    }

    const auto chunkID = BinaryReader::read<u32>(file);
    const auto fourCC = BinaryReader::read<u32>(file.subspan(8, 4));

    if (chunkID != makeFourCC('R', 'I', 'F', 'F')) {
        return false;
    }
    if (fourCC != makeFourCC('W', 'A', 'V', 'E')) {
        return false;
    }
    return true;
}

[[nodiscard]] bool
isOggVorbisFormat(std::span<const u8> file) noexcept
{
    if (file.size() < 12) {
        return false;
    }

    const auto chunkID = BinaryReader::read<u32>(file);

    if (chunkID != makeFourCC('O', 'g', 'g', 'S')) {
        return false;
    }
    return true;
}

} // namespace

[[nodiscard]] std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
loadAudioClip(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<AudioEngine>& audioEngine,
    std::string_view filePath) noexcept
{
    const std::string path{filePath};

    const auto make_result = [&](std::unique_ptr<Error>&& err) -> std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>> {
        return std::make_tuple(nullptr, std::move(err));
    };

    auto [fileInfo, statErr] = vfs::stat(fs, path);
    if (statErr != nullptr) {
        return make_result(errors::wrap(std::move(statErr), "failed to get file info, " + path));
    }

    if (fileInfo.size <= 0) {
        return make_result(errors::make("the audio file is too small, " + path));
    }

    auto [file, openErr] = vfs::open(fs, path);
    if (openErr != nullptr) {
        return make_result(errors::wrap(std::move(openErr), "failed to open file, " + path));
    }

    std::vector<u8> binary = {};
    binary.resize(fileInfo.size);

    constexpr bool debugFillMemory = true;
    if constexpr (debugFillMemory) {
        memsetSpan(std::span{binary}, u8(0xAC));
    }

    auto [bytesRead, readErr] = file->read(binary);
    if (readErr != nullptr) {
        return make_result(errors::wrap(std::move(readErr), "failed to read file, " + path));
    }

    if (isWAVFormat(binary)) {
        auto [container, decodeErr] = decodeWAV(binary);
        if (decodeErr != nullptr) {
            return make_result(errors::wrap(std::move(decodeErr), "decodeWAV() failed, " + path));
        }

        auto [audioClip, clipErr] = audioEngine->createAudioClip(
            container.audioData.data(),
            container.audioData.size(),
            container.sampleRate,
            container.bitsPerSample,
            container.channels);

        if (clipErr != nullptr) {
            return make_result(errors::wrap(std::move(clipErr), "audioEngine->createAudioClip() failed, " + path));
        }

        return std::make_tuple(std::move(audioClip), nullptr);
    }
    if (isOggVorbisFormat(binary)) {
        auto [container, decodeErr] = decodeOggVorbis(binary);
        if (decodeErr != nullptr) {
            return make_result(errors::wrap(std::move(decodeErr), "decodeOggVorbis() failed, " + path));
        }

        auto [audioClip, clipErr] = audioEngine->createAudioClip(
            container.audioData.data(),
            container.audioData.size(),
            container.sampleRate,
            container.bitsPerSample,
            container.channels);

        if (clipErr != nullptr) {
            return make_result(errors::wrap(std::move(clipErr), "audioEngine->createAudioClip() failed, " + path));
        }

        return std::make_tuple(std::move(audioClip), nullptr);
    }

    return make_result(errors::make("format unsupported, " + path));
}

} // namespace pomdog
