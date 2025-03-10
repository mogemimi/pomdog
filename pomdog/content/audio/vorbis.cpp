// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/audio/vorbis.h"
#include "pomdog/audio/audio_channels.h"
#include "pomdog/audio/audio_engine.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <stb/stb_vorbis.h>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::Vorbis {
namespace {

[[nodiscard]] AudioChannels
toAudioChannels(int channels) noexcept
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
load(const std::shared_ptr<AudioEngine>& audioEngine, const std::string& filename) noexcept
{
    int error = 0;
    auto vorbis = stb_vorbis_open_filename(filename.data(), &error, nullptr);

    if (error != 0) {
        auto err = errors::make("failed to read ogg/vorbis file " + filename);
        return std::make_tuple(nullptr, std::move(err));
    }

    auto info = stb_vorbis_get_info(vorbis);
    if (info.channels <= 0) {
        return std::make_tuple(nullptr, errors::make("info.channels must be > 0, " + filename));
    }

    auto channels = toAudioChannels(info.channels);
    int totalSamples = static_cast<int>(stb_vorbis_stream_length_in_samples(vorbis));

    if (totalSamples <= 0) {
        return std::make_tuple(nullptr, errors::make("totalSamples must be > 0, " + filename));
    }

    std::vector<std::uint8_t> audioData;
    audioData.resize(
        sizeof(std::uint16_t) *
        static_cast<std::size_t>(totalSamples) *
        static_cast<std::size_t>(info.channels));

    int sampleCount = stb_vorbis_get_samples_short_interleaved(
        vorbis,
        info.channels,
        reinterpret_cast<short*>(audioData.data()),
        totalSamples);

    if (sampleCount < totalSamples) {
        audioData.resize(
            sizeof(std::uint16_t) *
            static_cast<std::size_t>(sampleCount) *
            static_cast<std::size_t>(info.channels));
    }

    const auto samplesPerSec = static_cast<int>(info.sample_rate);
    constexpr int bitsPerSample = 16;

    // NOTE: Clean up vorbis file
    stb_vorbis_close(vorbis);

    if (audioEngine == nullptr) {
        auto err = errors::make("audioEngine is null.");
        return std::make_tuple(nullptr, std::move(err));
    }
    POMDOG_ASSERT(audioEngine != nullptr);

    // NOTE: Create audio clip.
    auto [audioClip, audioClipErr] = audioEngine->createAudioClip(
        audioData.data(),
        audioData.size(),
        samplesPerSec,
        bitsPerSample,
        channels);

    if (audioClipErr != nullptr) {
        auto err = errors::wrap(std::move(audioClipErr), "CreateAudioClip() failed.");
        return std::make_tuple(nullptr, std::move(err));
    }

    return std::make_tuple(std::move(audioClip), nullptr);
}

} // namespace pomdog::Vorbis
