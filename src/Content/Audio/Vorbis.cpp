// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/Audio/Vorbis.hpp"
#include "Pomdog/Audio/AudioChannels.hpp"
#include "Pomdog/Audio/AudioEngine.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <stb_vorbis.h>
#include <utility>

namespace Pomdog::Vorbis {
namespace {

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
Load(const std::shared_ptr<AudioEngine>& audioEngine, const std::string& filename) noexcept
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

    if (audioEngine == nullptr) {
        auto err = Errors::New("audioEngine is null.");
        return std::make_tuple(nullptr, std::move(err));
    }
    POMDOG_ASSERT(audioEngine != nullptr);

    // NOTE: Create audio clip.
    auto [audioClip, audioClipErr] = audioEngine->CreateAudioClip(
        audioData.data(),
        audioData.size(),
        samplesPerSec,
        bitsPerSample,
        channels);

    if (audioClipErr != nullptr) {
        auto err = Errors::Wrap(std::move(audioClipErr), "CreateAudioClip() failed.");
        return std::make_tuple(nullptr, std::move(err));
    }

    return std::make_tuple(std::move(audioClip), nullptr);
}

} // namespace Pomdog::Vorbis
