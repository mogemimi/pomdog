// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/Audio/Vorbis.hpp"

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
#include "Pomdog/Utility/Assert.hpp"

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wconditional-uninitialized"
#pragma clang diagnostic ignored "-Wshadow"
#endif
#if defined(__GNUC__)
#pragma GCC diagnostic push
#if defined(__has_warning)
#if __has_warning("-Wmaybe-uninitialized")
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#elif !defined(__clang__)
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#pragma GCC diagnostic ignored "-Wunused-value"
#endif
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4100 4244 4245 4456 4457 4701)
#endif
#include <stb_vorbis.c>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <utility>

namespace Pomdog::Vorbis {
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

[[nodiscard]] AudioChannels ToAudioChannels(int channels) noexcept
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

std::tuple<std::shared_ptr<AudioClip>, std::shared_ptr<Error>>
Load(const std::string& filename)
{
    int error = 0;
    auto vorbis = stb_vorbis_open_filename(filename.data(), &error, nullptr);

    if (error != 0) {
        auto err = Errors::New("failed to read ogg/vorbis file " + filename);
        return std::make_tuple(nullptr, std::move(err));
    }

    auto info = stb_vorbis_get_info(vorbis);

    auto channels = ToAudioChannels(info.channels);
    int totalSamples = static_cast<int>(stb_vorbis_stream_length_in_samples(vorbis));

    std::vector<std::uint8_t> audioData;
    audioData.resize(sizeof(std::uint16_t) * totalSamples * info.channels);

    int sampleCount = stb_vorbis_get_samples_short_interleaved(
        vorbis,
        info.channels,
        reinterpret_cast<short*>(audioData.data()),
        totalSamples);

    if (sampleCount < totalSamples) {
        audioData.resize(sizeof(std::uint16_t) * sampleCount * info.channels);
    }

    const int samplesPerSec = info.sample_rate;
    constexpr int bitsPerSample = 16;

    // NOTE: Clean up vorbis file
    stb_vorbis_close(vorbis);

#if defined(POMDOG_PLATFORM_MACOSX) \
    || defined(POMDOG_PLATFORM_APPLE_IOS) \
    || defined(POMDOG_PLATFORM_LINUX)
    using Detail::SoundSystem::OpenAL::AudioClipAL;
    auto nativeAudioClip = std::make_unique<AudioClipAL>(
        audioData.data(),
        audioData.size(),
        samplesPerSec,
        bitsPerSample,
        channels);

#elif defined(POMDOG_PLATFORM_WIN32) \
    || defined(POMDOG_PLATFORM_XBOX_ONE)
    // NOTE: WAVE_FORMAT_PCM
    constexpr std::uint16_t WaveFormatTagPCM = 0x0001;

    const auto blockAlign = (info.channels * bitsPerSample) / 8;

    PCMWaveFormat waveFormat;
    waveFormat.FormatTag = WaveFormatTagPCM;
    waveFormat.Channels = static_cast<std::uint16_t>(info.channels);
    waveFormat.AvgBytesPerSec = blockAlign * samplesPerSec;
    waveFormat.SamplesPerSec = samplesPerSec;
    waveFormat.BlockAlign = static_cast<std::uint16_t>(blockAlign);
    waveFormat.BitsPerSample = bitsPerSample;

    std::uint16_t extraBytes = 0;

    std::vector<std::uint8_t> waveFormatData;
    waveFormatData.resize(sizeof(waveFormat) + sizeof(extraBytes));

    std::memcpy(waveFormatData.data(), &waveFormat, sizeof(waveFormat));
    std::memcpy(waveFormatData.data() + sizeof(waveFormat), &extraBytes, sizeof(extraBytes));

    using Detail::SoundSystem::XAudio2::AudioClipXAudio2;
    auto nativeAudioClip = std::make_unique<AudioClipXAudio2>(
        std::move(audioData),
        std::move(waveFormatData));
#endif

    auto audioClip = std::make_shared<AudioClip>(
        std::move(nativeAudioClip),
        samplesPerSec,
        bitsPerSample,
        channels);

    return std::make_tuple(std::move(audioClip), nullptr);
}

} // namespace Pomdog::Vorbis
