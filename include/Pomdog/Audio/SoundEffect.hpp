// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SOUNDEFFECT_0793F2D9_5D45_44A0_BA2A_66B75D99D015_HPP
#define POMDOG_SOUNDEFFECT_0793F2D9_5D45_44A0_BA2A_66B75D99D015_HPP

#include "detail/ForwardDeclarations.hpp"
#include "SoundState.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <cstdint>

namespace Pomdog {

class AudioClip;
class AudioEmitter;
class AudioListener;
class AudioEngine;
enum class SoundState: std::uint8_t;

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

}// namespace Pomdog

#endif // !defined(POMDOG_SOUNDEFFECT_0793F2D9_5D45_44A0_BA2A_66B75D99D015_HPP)
