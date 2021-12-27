// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/audio_engine.h"
#include "pomdog/audio/openal/prerequisites_openal.h"
#include "pomdog/utility/errors.h"
#include <memory>

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::openal {

class AudioEngineAL final : public AudioEngine {
private:
    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;

public:
    AudioEngineAL() noexcept;

    AudioEngineAL(const AudioEngineAL&) = delete;
    AudioEngineAL& operator=(const AudioEngineAL&) = delete;

    ~AudioEngineAL() noexcept override;

    /// Initializes the audio engine.
    [[nodiscard]] std::unique_ptr<Error>
    Initialize() noexcept;

    /// Creates an audio clip.
    [[nodiscard]] std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
    CreateAudioClip(
        const void* audioData,
        std::size_t sizeInBytes,
        int sampleRate,
        int bitsPerSample,
        AudioChannels channels) noexcept override;

    /// Creates a sound effect.
    [[nodiscard]] std::tuple<std::shared_ptr<SoundEffect>, std::unique_ptr<Error>>
    CreateSoundEffect(
        const std::shared_ptr<AudioClip>& audioClip,
        bool isLooped) noexcept override;

    /// Gets the main volume that affects all sound effects.
    [[nodiscard]] float
    GetMainVolume() const noexcept override;

    /// Sets the main volume that affects all sound effects.
    void
    SetMainVolume(float volume) noexcept override;
};

} // namespace pomdog::detail::openal
