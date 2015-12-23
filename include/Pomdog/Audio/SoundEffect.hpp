// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Audio/detail/ForwardDeclarations.hpp"
#include "Pomdog/Audio/SoundState.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <cstdint>

namespace Pomdog {

class POMDOG_EXPORT SoundEffect {
public:
    SoundEffect(AudioEngine & audioEngine,
        std::shared_ptr<AudioClip> const& audioClip, bool isLooped);

    ~SoundEffect();

    SoundEffect(SoundEffect const&) = delete;
    SoundEffect(SoundEffect &&) = default;
    SoundEffect & operator=(SoundEffect const&) = delete;
    SoundEffect & operator=(SoundEffect &&) = default;

    void Pause();

    void Play();

    void Stop();

    void Apply3D(AudioListener const& listener, AudioEmitter const& emitter);

    bool IsLooped() const;

    void ExitLoop();

    SoundState State() const;

    float Pan() const;

    void Pan(float panpot);

    float Pitch() const;

    void Pitch(float pitch);

    float Volume() const;

    void Volume(float volume);

private:
    std::unique_ptr<Detail::SoundSystem::NativeSoundEffect> nativeSoundEffect;
    float pitch;
    float volume;
    SoundState state;
    bool isLooped;
};

} // namespace Pomdog
