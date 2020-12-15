// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Audio/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>
#include <tuple>

namespace Pomdog {

class Error;

class POMDOG_EXPORT AudioEngine {
public:
    virtual ~AudioEngine() noexcept;

    /// Creates an audio clip.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
    CreateAudioClip(
        const void* audioData,
        std::size_t sizeInBytes,
        int sampleRate,
        int bitsPerSample,
        AudioChannels channels) noexcept = 0;

    /// Creates a sound effect.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<SoundEffect>, std::unique_ptr<Error>>
    CreateSoundEffect(
        const std::shared_ptr<AudioClip>& audioClip,
        bool isLooped) noexcept = 0;

    /// Gets the main volume that affects all sound effects.
    [[nodiscard]] virtual float
    GetMainVolume() const noexcept = 0;

    /// Sets the main volume that affects all sound effects.
    virtual void
    SetMainVolume(float volume) noexcept = 0;
};

} // namespace Pomdog
