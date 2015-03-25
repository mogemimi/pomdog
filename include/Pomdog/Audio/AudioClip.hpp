// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_AUDIOCLIP_C1887233_F337_44F3_8AAC_784260497BCE_HPP
#define POMDOG_AUDIOCLIP_C1887233_F337_44F3_8AAC_784260497BCE_HPP

#if _MSC_VER > 1000
#pragma once
#endif

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
        std::uint32_t sampleRate, std::uint16_t bitsPerSample, AudioChannels channels);

    AudioClip(AudioClip const&) = delete;
    AudioClip & operator=(AudioClip const&) = delete;

    ~AudioClip();

    Duration Length() const;

    std::uint32_t SampleRate() const;

    std::uint16_t BitsPerSample() const;

    AudioChannels Channels() const;

    Detail::SoundSystem::NativeAudioClip* NativeAudioClip();

private:
    std::unique_ptr<Detail::SoundSystem::NativeAudioClip> nativeAudioClip;
    std::uint32_t sampleRate;
    std::uint16_t bitsPerSample;
    AudioChannels channels;
};

}// namespace Pomdog

#endif // !defined(POMDOG_AUDIOCLIP_C1887233_F337_44F3_8AAC_784260497BCE_HPP)
