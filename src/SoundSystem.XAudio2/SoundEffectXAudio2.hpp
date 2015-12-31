// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesXAudio2.hpp"
#include "../Utility/Noncopyable.hpp"
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

class SoundEffectXAudio2 final : Noncopyable {
public:
    SoundEffectXAudio2(AudioEngineXAudio2 & audioEngine,
        std::shared_ptr<AudioClipXAudio2> const& audioClip, bool isLooped);

    ~SoundEffectXAudio2();

    void ExitLoop();

    void Pause();

    void Play();

    void Stop();

    void Apply3D(AudioListener const& listener, AudioEmitter const& emitter);

    void Pitch(float pitch);

    void Volume(float volume);

private:
    std::shared_ptr<AudioClipXAudio2> audioClip;
    IXAudio2SourceVoice* sourceVoice;
    bool isLooped;
};

} // namespace XAudio2
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog
