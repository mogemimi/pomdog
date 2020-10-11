// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesXAudio2.hpp"
#include "Pomdog/Audio/AudioEngine.hpp"
#include <wrl/client.h>
#include <memory>

namespace Pomdog {
class Error;
} // namespace Pomdog

namespace Pomdog::Detail::XAudio2 {

class AudioEngineXAudio2 final : public AudioEngine {
private:
    Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
    IXAudio2MasteringVoice* mainVoice = nullptr;

public:
    AudioEngineXAudio2() noexcept;

    ~AudioEngineXAudio2() noexcept;

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

    /// Gets the pointer of the IXAudio2 object.
    [[nodiscard]] IXAudio2*
    GetXAudio2Engine() const noexcept;
};

} // namespace Pomdog::Detail::XAudio2
