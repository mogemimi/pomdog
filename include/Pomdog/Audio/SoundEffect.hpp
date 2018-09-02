// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Audio/SoundState.hpp"
#include "Pomdog/Audio/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT SoundEffect final {
public:
    SoundEffect(
        AudioEngine& audioEngine,
        const std::shared_ptr<AudioClip>& audioClip,
        bool isLooped);

    ~SoundEffect();

    SoundEffect(const SoundEffect&) = delete;
    SoundEffect(SoundEffect&&) = default;
    SoundEffect& operator=(const SoundEffect&) = delete;
    SoundEffect& operator=(SoundEffect&&) = default;

    void Pause();

    void Play();

    void Stop();

    void Apply3D(const AudioListener& listener, const AudioEmitter& emitter);

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
