// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Audio/AudioChannels.hpp"
#include "Pomdog/Audio/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT AudioClip final {
public:
    AudioClip(
        std::unique_ptr<Detail::SoundSystem::NativeAudioClip>&& nativeAudioClip,
        int sampleRate,
        int bitsPerSample,
        AudioChannels channels);

    AudioClip(const AudioClip&) = delete;
    AudioClip& operator=(const AudioClip&) = delete;

    ~AudioClip();

    /// Gets the length of the audio clip in seconds.
    Duration GetLength() const;

    /// Gets the number of samples per second.
    int GetSampleRate() const;

    /// Gets the number of bits per sample.
    int GetBitsPerSample() const;

    /// Gets the number of channels in the audip clip.
    AudioChannels GetChannels() const;

    /// Gets the pointer of the native audio clip.
    Detail::SoundSystem::NativeAudioClip* GetNativeAudioClip();

private:
    std::unique_ptr<Detail::SoundSystem::NativeAudioClip> nativeAudioClip;
    std::int32_t sampleRate;
    std::int32_t bitsPerSample;
    AudioChannels channels;
};

} // namespace Pomdog
