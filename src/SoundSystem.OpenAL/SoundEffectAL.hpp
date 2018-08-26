// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesOpenAL.hpp"
#include "../Utility/Noncopyable.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <memory>

namespace Pomdog {
class AudioEmitter;
class AudioListener;
} // namespace Pomdog

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace OpenAL {

struct AudioSourceALTag {};
using AudioSourceAL = Tagged<ALuint, AudioSourceALTag>;

class AudioClipAL;
class AudioEngineAL;

class SoundEffectAL final : Noncopyable {
private:
    std::shared_ptr<AudioClipAL> audioClip;
    Optional<AudioSourceAL> source;

public:
    SoundEffectAL(
        AudioEngineAL & audioEngine,
        const std::shared_ptr<AudioClipAL>& audioClip,
        bool isLooped);

    ~SoundEffectAL();

    void Pause();

    void Play();

    void Stop();

    void Apply3D(const AudioListener& listener, const AudioEmitter& emitter);

    void ExitLoop();

    void SetPitch(float pitch);

    void SetVolume(float volume);
};

} // namespace OpenAL
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog
