// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesXAudio2.hpp"
#include <memory>

namespace Pomdog {
class AudioEmitter;
class AudioListener;
} // namespace Pomdog

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace XAudio2 {

class AudioClipXAudio2;
class AudioEngineXAudio2;

class SoundEffectXAudio2 final {
public:
    SoundEffectXAudio2(
        AudioEngineXAudio2 & audioEngine,
        const std::shared_ptr<AudioClipXAudio2>& audioClip,
        bool isLooped);

    SoundEffectXAudio2(const SoundEffectXAudio2&) = delete;
    SoundEffectXAudio2& operator=(const SoundEffectXAudio2&) = delete;

    ~SoundEffectXAudio2();

    void ExitLoop();

    void Pause();

    void Play();

    void Stop();

    void Apply3D(const AudioListener& listener, const AudioEmitter& emitter);

    void SetPitch(float pitch);

    void SetVolume(float volume);

private:
    std::shared_ptr<AudioClipXAudio2> audioClip;
    IXAudio2SourceVoice* sourceVoice;
    bool isLooped;
};

} // namespace XAudio2
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog
