// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/audio_engine.h"
#include "pomdog/audio/xaudio2/prerequisites_xaudio2.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::xaudio2 {

class AudioEngineXAudio2 final : public AudioEngine {
private:
    Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
    IXAudio2MasteringVoice* mainVoice = nullptr;

public:
    AudioEngineXAudio2() noexcept;

    ~AudioEngineXAudio2() noexcept;

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

    /// Gets the pointer of the IXAudio2 object.
    [[nodiscard]] IXAudio2*
    GetXAudio2Engine() const noexcept;
};

} // namespace pomdog::detail::xaudio2
