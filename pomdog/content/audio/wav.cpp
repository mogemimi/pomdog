// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/audio/wav.h"
#include "pomdog/audio/audio_channels.h"
#include "pomdog/audio/audio_engine.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/content/utility/binary_reader.h"
#include "pomdog/content/utility/make_fourcc.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace BinaryReader = pomdog::detail::BinaryReader;
using pomdog::detail::makeFourCC;

namespace pomdog::WAV {
namespace {

#if defined(POMDOG_INTERNAL_PACKED)
#error "'POMDOG_INTERNAL_PACKED' already defined."
#elif defined(_MSC_VER)
#define POMDOG_INTERNAL_PACKED(type) __pragma(pack(push, 1)) type __pragma(pack(pop))
#elif defined(__clang__) || defined(__GNUC__)
#define POMDOG_INTERNAL_PACKED(type) type __attribute__((packed))
#else
#error "'POMDOG_INTERNAL_PACKED' is not supported in this compiler."
#endif

POMDOG_INTERNAL_PACKED(struct RIFFChunk final {
    std::uint32_t ChunkID;
    std::uint32_t ChunkSize;
    std::uint32_t FourCCType;
});

POMDOG_INTERNAL_PACKED(struct SubChunkHeader final {
    std::uint32_t ChunkID;
    std::uint32_t ChunkSize;
});

POMDOG_INTERNAL_PACKED(struct PCMWaveFormat final {
    std::uint16_t FormatTag;
    std::uint16_t Channels;
    std::uint32_t SamplesPerSec;
    std::uint32_t AvgBytesPerSec;
    std::uint16_t BlockAlign;
    std::uint16_t BitsPerSample;
});

struct WaveFormat final {
    PCMWaveFormat PCMFormat;
    std::uint16_t ExtraBytes = 0;
    std::vector<std::uint8_t> ExtraData;
};

[[nodiscard]] std::unique_ptr<Error>
readRIFFChunk(std::ifstream& stream)
{
    const auto riffChunk = BinaryReader::read<RIFFChunk>(stream);

    if (stream.fail()) {
        return errors::make("failed to read read RiffChunk");
    }

    if (riffChunk.ChunkID != makeFourCC('R', 'I', 'F', 'F')) {
        return errors::make("missing RIFF chunk");
    }
    if (riffChunk.FourCCType != makeFourCC('W', 'A', 'V', 'E')) {
        return errors::make("FourCCType is not 'WAVE'");
    }

    return nullptr;
}

[[nodiscard]] std::tuple<WaveFormat, std::unique_ptr<Error>>
readWaveFormat(std::ifstream& stream)
{
    const auto fmtChunkHeader = BinaryReader::read<SubChunkHeader>(stream);

    if (stream.fail()) {
        auto err = errors::make("failed to read SubChunkHeader");
        return std::make_tuple(WaveFormat{}, std::move(err));
    }

    if (fmtChunkHeader.ChunkID != makeFourCC('f', 'm', 't', ' ')) {
        auto err = errors::make("cannot find the 'fmt ' chunk");
        return std::make_tuple(WaveFormat{}, std::move(err));
    }
    if (fmtChunkHeader.ChunkSize < sizeof(PCMWaveFormat)) {
        auto err = errors::make("The 'fmt ' chunk size is too small: " + std::to_string(fmtChunkHeader.ChunkSize));
        return std::make_tuple(WaveFormat{}, std::move(err));
    }

    WaveFormat waveFormat;
    waveFormat.PCMFormat = BinaryReader::read<PCMWaveFormat>(stream);
    waveFormat.ExtraBytes = 0;

    if (stream.fail()) {
        auto err = errors::make("failed to read the 'fmt ' chunk into PCMWaveFormat");
        return std::make_tuple(std::move(waveFormat), std::move(err));
    }

    // NOTE: WAVE_FORMAT_PCM
    constexpr std::uint16_t WaveFormatTagPCM = 0x0001;

    if (waveFormat.PCMFormat.FormatTag == WaveFormatTagPCM) {
        waveFormat.ExtraBytes = 0;
        waveFormat.ExtraData.clear();
    }
    else {
        waveFormat.ExtraBytes = BinaryReader::read<std::uint16_t>(stream);
        if (stream.fail()) {
            auto err = errors::make("failed to read the extra bytes");
            return std::make_tuple(std::move(waveFormat), std::move(err));
        }

        waveFormat.ExtraData.resize(waveFormat.ExtraBytes);
        stream.read(reinterpret_cast<char*>(waveFormat.ExtraData.data()), waveFormat.ExtraBytes);

        if (stream.fail()) {
            auto err = errors::make("failed to read the extra data");
            return std::make_tuple(std::move(waveFormat), std::move(err));
        }
    }

    return std::make_tuple(std::move(waveFormat), nullptr);
}

[[nodiscard]] std::tuple<std::vector<std::uint8_t>, std::unique_ptr<Error>>
readWaveAudioData(std::ifstream& stream)
{
    POMDOG_ASSERT(stream);

    const auto chunkHeader = BinaryReader::read<SubChunkHeader>(stream);

    if (stream.fail()) {
        auto err = errors::make("failed to read 'data' chunk header");
        return std::make_tuple(std::vector<std::uint8_t>{}, std::move(err));
    }
    if (chunkHeader.ChunkID != makeFourCC('d', 'a', 't', 'a')) {
        auto err = errors::make("cannot find the 'data' chunk");
        return std::make_tuple(std::vector<std::uint8_t>{}, std::move(err));
    }
    if (chunkHeader.ChunkSize <= 0) {
        auto err = errors::make("data chunk is empty");
        return std::make_tuple(std::vector<std::uint8_t>{}, std::move(err));
    }

    std::vector<std::uint8_t> audioData;
    audioData.resize(chunkHeader.ChunkSize);

    for (auto& dest : audioData) {
        dest = BinaryReader::read<std::uint8_t>(stream);
        if (stream.fail()) {
            auto err = errors::make("failed to read wave data");
            return std::make_tuple(std::move(audioData), std::move(err));
        }
    }

    return std::make_tuple(std::move(audioData), nullptr);
}

[[nodiscard]] AudioChannels
toAudioChannels(std::uint32_t channels) noexcept
{
    POMDOG_ASSERT(channels > 0);
    POMDOG_ASSERT(channels <= 2);

    switch (channels) {
    case 1:
        return AudioChannels::Mono;
    case 2:
        return AudioChannels::Stereo;
    default:
        break;
    }
    return AudioChannels::Mono;
}

} // namespace

[[nodiscard]] std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
load(const std::shared_ptr<AudioEngine>& audioEngine, std::ifstream&& stream, std::size_t byteLength) noexcept
{
    constexpr auto MinimumWaveFormatSizeInBytes = 4 * 11;
    if (byteLength < MinimumWaveFormatSizeInBytes) {
        return std::make_tuple(nullptr, errors::make("the wave file is too small"));
    }

    if (!stream) {
        return std::make_tuple(nullptr, errors::make("invalid file stream"));
    }

    if (auto readErr = readRIFFChunk(stream); readErr != nullptr) {
        auto err = errors::wrap(std::move(readErr), "cannot read the wave file");
        return std::make_tuple(nullptr, std::move(err));
    }

    auto [waveFormat, waveFormatErr] = readWaveFormat(stream);
    if (waveFormatErr != nullptr) {
        auto err = errors::wrap(std::move(waveFormatErr), "cannot read the wave format");
        return std::make_tuple(nullptr, std::move(err));
    }

    auto [audioData, audioDataErr] = readWaveAudioData(stream);
    if (audioDataErr != nullptr) {
        auto err = errors::wrap(std::move(audioDataErr), "cannot read the wave audio data");
        return std::make_tuple(nullptr, std::move(err));
    }

    auto channels = toAudioChannels(waveFormat.PCMFormat.Channels);

    if (audioEngine == nullptr) {
        auto err = errors::make("audioEngine is null.");
        return std::make_tuple(nullptr, std::move(err));
    }
    POMDOG_ASSERT(audioEngine != nullptr);

    // NOTE: Create audio clip.
    auto [audioClip, audioClipErr] = audioEngine->createAudioClip(
        audioData.data(),
        audioData.size(),
        static_cast<int>(waveFormat.PCMFormat.SamplesPerSec),
        static_cast<int>(waveFormat.PCMFormat.BitsPerSample),
        channels);

    if (audioClipErr != nullptr) {
        auto err = errors::wrap(std::move(audioClipErr), "CreateAudioClip() failed.");
        return std::make_tuple(nullptr, std::move(err));
    }

    return std::make_tuple(std::move(audioClip), nullptr);
}

} // namespace pomdog::WAV
