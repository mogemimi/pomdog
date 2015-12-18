// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/ForwardDeclarations.hpp"
#include "AudioChannels.hpp"
#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT AudioClip {
public:
    AudioClip(std::unique_ptr<Detail::SoundSystem::NativeAudioClip> && nativeAudioClip,
        int sampleRate, int bitsPerSample, AudioChannels channels);

    AudioClip(AudioClip const&) = delete;
    AudioClip & operator=(AudioClip const&) = delete;

    ~AudioClip();

    Duration Length() const;

    int SampleRate() const;

    int BitsPerSample() const;

    AudioChannels Channels() const;

    Detail::SoundSystem::NativeAudioClip* NativeAudioClip();

private:
    std::unique_ptr<Detail::SoundSystem::NativeAudioClip> nativeAudioClip;
    std::int32_t sampleRate;
    std::int32_t bitsPerSample;
    AudioChannels channels;
};

} // namespace Pomdog
