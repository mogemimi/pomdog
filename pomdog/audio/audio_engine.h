// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/forward_declarations.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

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

} // namespace pomdog
