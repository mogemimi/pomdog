// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Audio/detail/ForwardDeclarations.hpp"
#include "Pomdog/Audio/SoundState.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <cstdint>

namespace Pomdog {

class POMDOG_EXPORT SoundEffect final {
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

    SoundState GetState() const;

    float GetPan() const;

    void SetPan(float panpot);

    float GetPitch() const;

    void SetPitch(float pitch);

    float GetVolume() const;

    void SetVolume(float volume);

private:
    std::unique_ptr<Detail::SoundSystem::NativeSoundEffect> nativeSoundEffect;
    float pitch;
    float volume;
    SoundState state;
    bool isLooped;
};

} // namespace Pomdog
