// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/audio_engine.h"
#include "pomdog/audio/openal/openal_headers.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/memory/unsafe_ptr.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::openal {
class AudioSourceStreamingAL;
} // namespace pomdog::detail::openal

namespace pomdog::detail::openal {

class AudioEngineAL final : public AudioEngine {
private:
    std::vector<u8> streamBuffer_ = {};
    std::vector<std::weak_ptr<AudioSourceStreamingAL>> streamSources_ = {};
    unsafe_ptr<ALCdevice> device_ = nullptr;
    unsafe_ptr<ALCcontext> context_ = nullptr;

public:
    AudioEngineAL() noexcept;

    AudioEngineAL(const AudioEngineAL&) = delete;
    AudioEngineAL& operator=(const AudioEngineAL&) = delete;

    ~AudioEngineAL() noexcept override;

    /// Initializes the audio engine.
    [[nodiscard]] std::unique_ptr<Error>
    initialize() noexcept;

    /// Makes the OpenAL context current for the calling thread.
    void
    makeCurrentContext() noexcept;

    /// Clears the current OpenAL context.
    void
    clearCurrentContext() noexcept;

    /// Polls streaming audio sources, refilling buffers as needed.
    void
    update() noexcept;

    /// Creates a non-streaming audio clip from raw PCM data.
    [[nodiscard]] std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
    createAudioClip(
        std::span<const u8> waveData,
        i32 sampleRate,
        i32 bitsPerSample,
        AudioChannels channels) noexcept override;

    /// Creates a streaming audio clip from an audio clip file.
    [[nodiscard]] std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
    createAudioClip(
        std::unique_ptr<detail::AudioClipFile> audioClipFile) noexcept override;

    /// Creates an audio source for playing back the specified audio clip.
    [[nodiscard]] std::tuple<std::shared_ptr<AudioSource>, std::unique_ptr<Error>>
    createAudioSource(
        const std::shared_ptr<AudioClip>& audioClip,
        bool isLooped) noexcept override;

    /// Gets the main volume that affects all audio output.
    [[nodiscard]] f32
    getMainVolume() const noexcept override;

    /// Sets the main volume that affects all audio output.
    void
    setMainVolume(f32 volume) noexcept override;
};

} // namespace pomdog::detail::openal
