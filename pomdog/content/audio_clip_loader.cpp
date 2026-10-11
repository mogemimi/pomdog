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
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
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

    auto [file, openErr] = vfs::open(fs, path);
    if (openErr != nullptr) {
        return make_result(errors::wrap(std::move(openErr), "failed to open file, " + path));
    }

    constexpr std::size_t maxFileSize = 1ULL * 1024 * 1024 * 1024; // 1 GB
    auto [size, sizeErr] = file->getSize();
    if (sizeErr != nullptr) {
        return make_result(errors::wrap(std::move(sizeErr), "failed to get file size: " + path));
    }
    if (size > maxFileSize) {
        return make_result(errors::make("the audio file is too large, " + path));
    }

    // NOTE: Uncompressed entries of a memory-mapped archive serve this view
    // without copying the file bytes.
    auto [binary, viewErr] = file->view();
    if (viewErr != nullptr) {
        return make_result(errors::wrap(std::move(viewErr), "failed to read file, " + path));
    }

    if (binary.empty()) {
        return make_result(errors::make("the audio file is too small, " + path));
    }

    if (binary.size() > maxFileSize) {
        return make_result(errors::make("the audio file is too large, " + path));
    }

    if (isWAVFormat(binary)) {
        auto [container, decodeErr] = decodeWAV(binary);
        if (decodeErr != nullptr) {
            return make_result(errors::wrap(std::move(decodeErr), "decodeWAV() failed, " + path));
        }

        auto [audioClip, clipErr] = audioEngine->createAudioClip(
            std::span<const u8>{container.audioData},
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

        // NOTE: Hand over the decoded samples, so that the engine keeps them
        // without copying them where it can.
        auto [audioClip, clipErr] = audioEngine->createAudioClip(
            std::move(container.rawData),
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

[[nodiscard]] std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
loadAudioClipStreaming(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<AudioEngine>& audioEngine,
    std::string_view filePath) noexcept
{
    const std::string path{filePath};

    const auto make_result = [&](std::unique_ptr<Error>&& err) -> std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>> {
        return std::make_tuple(nullptr, std::move(err));
    };

    auto [file, openErr] = vfs::open(fs, path);
    if (openErr != nullptr) {
        return make_result(errors::wrap(std::move(openErr), "failed to open file, " + path));
    }

    constexpr std::size_t maxFileSize = 1ULL * 1024 * 1024 * 1024; // 1 GB
    auto [size, sizeErr] = file->getSize();
    if (sizeErr != nullptr) {
        return make_result(errors::wrap(std::move(sizeErr), "failed to get file size: " + path));
    }
    if (size > maxFileSize) {
        return make_result(errors::make("the audio file is too large, " + path));
    }

    // NOTE: The streaming decoders keep the file open and decode from its view,
    // so viewing it here to detect the format reads nothing extra. Compressed
    // archive entries decode once into the buffer behind the view.
    auto [binary, viewErr] = file->view();
    if (viewErr != nullptr) {
        return make_result(errors::wrap(std::move(viewErr), "failed to read file, " + path));
    }

    if (binary.empty()) {
        return make_result(errors::make("the audio file is too small, " + path));
    }

    if (binary.size() > maxFileSize) {
        return make_result(errors::make("the audio file is too large, " + path));
    }

    if (isWAVFormat(binary)) {
        auto [clipFile, wavErr] = openAudioClipFileWAV(std::move(file));
        if (wavErr != nullptr) {
            return make_result(errors::wrap(std::move(wavErr), "openAudioClipFileWAV() failed, " + path));
        }

        auto [audioClip, clipErr] = audioEngine->createAudioClip(std::move(clipFile));
        if (clipErr != nullptr) {
            return make_result(errors::wrap(std::move(clipErr), "audioEngine->createAudioClip() failed, " + path));
        }

        return std::make_tuple(std::move(audioClip), nullptr);
    }
    if (isOggVorbisFormat(binary)) {
        auto [clipFile, oggErr] = openAudioClipFileOggVorbis(std::move(file));
        if (oggErr != nullptr) {
            return make_result(errors::wrap(std::move(oggErr), "openAudioClipFileOggVorbis() failed, " + path));
        }

        auto [audioClip, clipErr] = audioEngine->createAudioClip(std::move(clipFile));
        if (clipErr != nullptr) {
            return make_result(errors::wrap(std::move(clipErr), "audioEngine->createAudioClip() failed, " + path));
        }

        return std::make_tuple(std::move(audioClip), nullptr);
    }

    return make_result(errors::make("format unsupported, " + path));
}

} // namespace pomdog
