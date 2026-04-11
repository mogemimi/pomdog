// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <span>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class AudioClip;
class AudioSource;
class Error;
enum class AudioChannels : u8;
} // namespace pomdog

namespace pomdog::detail {
class AudioClipFile;
} // namespace pomdog::detail

namespace pomdog {

/// The audio engine manages audio resources and playback.
///
/// The AudioEngine is responsible for creating audio clips and audio sources,
/// and for controlling the main volume that affects all audio output.
///
/// There are two platform-specific implementations:
/// - OpenAL (macOS, Linux, Emscripten)
/// - XAudio2 (Windows)
class POMDOG_EXPORT AudioEngine {
public:
    AudioEngine() noexcept;

    AudioEngine(const AudioEngine&) = delete;
    AudioEngine& operator=(const AudioEngine&) = delete;

    virtual ~AudioEngine() noexcept;

    /// Creates a non-streaming audio clip from raw PCM audio data.
    /// @param waveData The raw PCM audio data.
    /// @param sampleRate The number of samples per second (e.g. 44100).
    /// @param bitsPerSample The number of bits per sample (8 or 16).
    /// @param channels The number of audio channels (Mono or Stereo).
    /// @return A tuple containing the audio clip and an error if one occurred.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
    createAudioClip(
        std::span<const u8> waveData,
        i32 sampleRate,
        i32 bitsPerSample,
        AudioChannels channels) noexcept = 0;

    /// Creates a streaming audio clip from an audio clip file.
    /// @param audioClipFile The audio clip file used for streaming.
    /// @return A tuple containing the audio clip and an error if one occurred.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
    createAudioClip(
        std::unique_ptr<detail::AudioClipFile> audioClipFile) noexcept = 0;

    /// Creates an audio source for playing back the specified audio clip.
    /// @param audioClip The audio clip to play.
    /// @param isLooped Whether the audio should loop when it reaches the end.
    /// @return A tuple containing the audio source and an error if one occurred.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<AudioSource>, std::unique_ptr<Error>>
    createAudioSource(
        const std::shared_ptr<AudioClip>& audioClip,
        bool isLooped) noexcept = 0;

    /// Gets the main volume that affects all audio output.
    [[nodiscard]] virtual f32
    getMainVolume() const noexcept = 0;

    /// Sets the main volume that affects all audio output.
    /// @param volume The volume value (0.0 = silent, 1.0 = full volume).
    virtual void
    setMainVolume(f32 volume) noexcept = 0;
};

} // namespace pomdog
