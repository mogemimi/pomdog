// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesOpenAL.hpp"
#include "Pomdog/Audio/AudioEngine.hpp"
#include <memory>

namespace Pomdog {
class Error;
} // namespace Pomdog

namespace Pomdog::Detail::OpenAL {

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
    [[nodiscard]] std::shared_ptr<Error>
    Initialize() noexcept;

    /// Creates an audio clip.
    [[nodiscard]] std::tuple<std::shared_ptr<AudioClip>, std::shared_ptr<Error>>
    CreateAudioClip(
        const void* audioData,
        std::size_t sizeInBytes,
        int sampleRate,
        int bitsPerSample,
        AudioChannels channels) noexcept override;

    /// Creates a sound effect.
    [[nodiscard]] std::tuple<std::shared_ptr<SoundEffect>, std::shared_ptr<Error>>
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

} // namespace Pomdog::Detail::OpenAL
