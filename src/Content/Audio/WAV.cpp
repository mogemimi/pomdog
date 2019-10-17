// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/Audio/WAV.hpp"

#include "Pomdog/Basic/Platform.hpp"
#if defined(POMDOG_PLATFORM_MACOSX) \
    || defined(POMDOG_PLATFORM_APPLE_IOS) \
    || defined(POMDOG_PLATFORM_LINUX)
#include "../../SoundSystem.OpenAL/AudioClipAL.hpp"
#elif defined(POMDOG_PLATFORM_WIN32) \
    || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "../../SoundSystem.XAudio2/AudioClipXAudio2.hpp"
#endif

#include "../../Basic/ConditionalCompilation.hpp"
#include "Pomdog/Audio/AudioClip.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Content/Utility/MakeFourCC.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cstdint>
#include <utility>
#include <vector>

using Pomdog::Detail::BinaryReader;
using Pomdog::Detail::MakeFourCC;

namespace Pomdog::WAV {
namespace {

#if defined(POMDOG_COMPILER_MSVC)
#pragma pack(push, 1)
#endif

#if defined(POMDOG_DETAIL_PACKED)
#    error "'POMDOG_DETAIL_PACKED' already defined."
#endif

#if defined(POMDOG_COMPILER_CLANG) || defined(POMDOG_COMPILER_GNUC)
#   define POMDOG_DETAIL_PACKED __attribute__((packed))
#elif defined(POMDOG_COMPILER_MSVC)
#    define POMDOG_DETAIL_PACKED
#else
#    error "'POMDOG_DETAIL_PACKED' is not supported in this compiler."
#endif

struct RIFFChunk final {
    std::uint32_t ChunkID;
    std::uint32_t ChunkSize;
    std::uint32_t FourCCType;
} POMDOG_DETAIL_PACKED;

struct SubChunkHeader final {
    std::uint32_t ChunkID;
    std::uint32_t ChunkSize;
} POMDOG_DETAIL_PACKED;

struct PCMWaveFormat final {
    std::uint16_t FormatTag;
    std::uint16_t Channels;
    std::uint32_t SamplesPerSec;
    std::uint32_t AvgBytesPerSec;
    std::uint16_t BlockAlign;
    std::uint16_t BitsPerSample;
} POMDOG_DETAIL_PACKED;

#if defined(POMDOG_DETAIL_PACKED)
#    undef POMDOG_DETAIL_PACKED
#endif

#if defined(POMDOG_COMPILER_MSVC)
#pragma pack(pop)
#endif

struct WaveFormat final {
    PCMWaveFormat PCMFormat;
    std::uint16_t ExtraBytes = 0;
    std::vector<std::uint8_t> ExtraData;
};

[[nodiscard]] std::shared_ptr<Error>
ReadRIFFChunk(std::ifstream& stream)
{
    const auto riffChunk = BinaryReader::Read<RIFFChunk>(stream);

    if (stream.fail()) {
        return Errors::New("failed to read read RiffChunk");
    }

    if (riffChunk.ChunkID != MakeFourCC('R', 'I', 'F', 'F')) {
        return Errors::New("missing RIFF chunk");
    }
    if (riffChunk.FourCCType != MakeFourCC('W', 'A', 'V', 'E')) {
        return Errors::New("FourCCType is not 'WAVE'");
    }

    return nullptr;
}

[[nodiscard]] std::tuple<WaveFormat, std::shared_ptr<Error>>
ReadWaveFormat(std::ifstream& stream)
{
    const auto fmtChunkHeader = BinaryReader::Read<SubChunkHeader>(stream);

    if (stream.fail()) {
        auto err = Errors::New("failed to read SubChunkHeader");
        return std::make_tuple(WaveFormat{}, std::move(err));
    }

    if (fmtChunkHeader.ChunkID != MakeFourCC('f', 'm', 't', ' ')) {
        auto err = Errors::New("cannot find the 'fmt ' chunk");
        return std::make_tuple(WaveFormat{}, std::move(err));
    }
    if (fmtChunkHeader.ChunkSize < sizeof(PCMWaveFormat)) {
        auto err = Errors::New("The 'fmt ' chunk size is too small: " + std::to_string(fmtChunkHeader.ChunkSize));
        return std::make_tuple(WaveFormat{}, std::move(err));
    }

    WaveFormat waveFormat;
    waveFormat.PCMFormat = BinaryReader::Read<PCMWaveFormat>(stream);
    waveFormat.ExtraBytes = 0;

    if (stream.fail()) {
        auto err = Errors::New("failed to read the 'fmt ' chunk into PCMWaveFormat");
        return std::make_tuple(std::move(waveFormat), std::move(err));
    }

    // NOTE: WAVE_FORMAT_PCM
    constexpr std::uint16_t WaveFormatTagPCM = 0x0001;

    if (waveFormat.PCMFormat.FormatTag == WaveFormatTagPCM) {
        waveFormat.ExtraBytes = 0;
        waveFormat.ExtraData.clear();
    }
    else {
        waveFormat.ExtraBytes = BinaryReader::Read<std::uint16_t>(stream);
        if (stream.fail()) {
            auto err = Errors::New("failed to read read the extra bytes");
            return std::make_tuple(std::move(waveFormat), std::move(err));
        }

        waveFormat.ExtraData.resize(waveFormat.ExtraBytes);
        stream.read(reinterpret_cast<char*>(waveFormat.ExtraData.data()), waveFormat.ExtraBytes);

        if (stream.fail()) {
            auto err = Errors::New("failed to read read the extra data");
            return std::make_tuple(std::move(waveFormat), std::move(err));
        }
    }

    return std::make_tuple(std::move(waveFormat), nullptr);
}

[[nodiscard]] std::tuple<std::vector<std::uint8_t>, std::shared_ptr<Error>>
ReadWaveAudioData(std::ifstream& stream)
{
    POMDOG_ASSERT(stream);

    const auto chunkHeader = BinaryReader::Read<SubChunkHeader>(stream);

    if (stream.fail()) {
        auto err = Errors::New("failed to read 'data' chunk header");
        return std::make_tuple(std::vector<std::uint8_t>{}, std::move(err));
    }
    if (chunkHeader.ChunkID != MakeFourCC('d', 'a', 't', 'a')) {
        auto err = Errors::New("cannot find the 'data' chunk");
        return std::make_tuple(std::vector<std::uint8_t>{}, std::move(err));
    }
    if (chunkHeader.ChunkSize <= 0) {
        auto err = Errors::New("data chunk is empty");
        return std::make_tuple(std::vector<std::uint8_t>{}, std::move(err));
    }

    std::vector<std::uint8_t> audioData;
    audioData.resize(chunkHeader.ChunkSize);

    for (auto& dest : audioData) {
        dest = BinaryReader::Read<std::uint8_t>(stream);
        if (stream.fail()) {
            auto err = Errors::New("failed to read wave data");
            return std::make_tuple(std::move(audioData), std::move(err));
        }
    }

    return std::make_tuple(std::move(audioData), nullptr);
}

[[nodiscard]] AudioChannels ToAudioChannels(std::uint32_t channels) noexcept
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

[[nodiscard]] std::tuple<std::shared_ptr<AudioClip>, std::shared_ptr<Error>>
Load(std::ifstream&& stream, std::size_t byteLength)
{
    constexpr auto MinimumWaveFormatSizeInBytes = 4 * 11;
    if (byteLength < MinimumWaveFormatSizeInBytes) {
        return std::make_tuple(nullptr, Errors::New("the wave file is too small"));
    }

    if (!stream) {
        return std::make_tuple(nullptr, Errors::New("invalid file stream"));
    }

    if (auto readErr = ReadRIFFChunk(stream); readErr != nullptr) {
        auto err = Errors::Wrap(std::move(readErr), "cannot read the wave file");
        return std::make_tuple(nullptr, std::move(err));
    }

    auto [waveFormat, waveFormatErr] = ReadWaveFormat(stream);
    if (waveFormatErr != nullptr) {
        auto err = Errors::Wrap(std::move(waveFormatErr), "cannot read the wave format");
        return std::make_tuple(nullptr, std::move(err));
    }

    auto [audioData, audioDataErr] = ReadWaveAudioData(stream);
    if (audioDataErr != nullptr) {
        auto err = Errors::Wrap(std::move(audioDataErr), "cannot read the wave audio data");
        return std::make_tuple(nullptr, std::move(err));
    }

    auto channels = ToAudioChannels(waveFormat.PCMFormat.Channels);

#if defined(POMDOG_PLATFORM_MACOSX) \
    || defined(POMDOG_PLATFORM_APPLE_IOS) \
    || defined(POMDOG_PLATFORM_LINUX)
    // NOTE: OpenAL
    using Detail::SoundSystem::OpenAL::AudioClipAL;
    auto nativeAudioClip = std::make_unique<AudioClipAL>(
        audioData.data(),
        audioData.size(),
        static_cast<int>(waveFormat.PCMFormat.SamplesPerSec),
        static_cast<int>(waveFormat.PCMFormat.BitsPerSample),
        channels);

#elif defined(POMDOG_PLATFORM_WIN32) \
    || defined(POMDOG_PLATFORM_XBOX_ONE)
    // NOTE: XAudio2
    std::vector<std::uint8_t> waveFormatData;
    waveFormatData.resize(sizeof(waveFormat.PCMFormat)
        + sizeof(waveFormat.ExtraBytes)
        + waveFormat.ExtraBytes);

    auto offset = sizeof(waveFormat.PCMFormat) + sizeof(waveFormat.ExtraBytes);
    std::memcpy(waveFormatData.data(), &waveFormat, offset);
    std::memcpy(waveFormatData.data() + offset, waveFormat.ExtraData.data(), waveFormat.ExtraBytes);

    using Detail::SoundSystem::XAudio2::AudioClipXAudio2;
    auto nativeAudioClip = std::make_unique<AudioClipXAudio2>(
        std::move(audioData),
        std::move(waveFormatData));
#endif

    auto audioClip = std::make_shared<AudioClip>(
        std::move(nativeAudioClip),
        static_cast<int>(waveFormat.PCMFormat.SamplesPerSec),
        static_cast<int>(waveFormat.PCMFormat.BitsPerSample),
        channels);

    return std::make_tuple(std::move(audioClip), nullptr);
}

} // namespace Pomdog::WAV
