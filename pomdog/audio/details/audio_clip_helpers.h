// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
enum class AudioChannels : u8;
} // namespace pomdog

namespace pomdog::detail {

/// Converts an AudioChannels enum value to the corresponding integer channel count.
/// @param channels The audio channel configuration (Mono or Stereo).
/// @return The number of channels: 1 for Mono, 2 for Stereo.
[[nodiscard]] POMDOG_EXPORT u8
toAudioChannelCount(AudioChannels channels) noexcept;

/// Converts an integer channel count to the corresponding AudioChannels enum value.
/// @param channelCount The number of audio channels (1 or 2).
/// @return The AudioChannels value, or std::nullopt if the channel count is unsupported.
[[nodiscard]] POMDOG_EXPORT std::optional<AudioChannels>
toAudioChannels(i32 channelCount) noexcept;

/// Calculates the number of samples per channel from the total size of audio data.
/// @param sizeInBytes The total size of the audio data in bytes.
/// @param bitsPerSample The number of bits per sample (8, 16, 24, or 32).
/// @param channels The audio channel configuration (Mono or Stereo).
/// @return The number of samples per channel.
[[nodiscard]] POMDOG_EXPORT i32
calculateSampleCount(i32 sizeInBytes, i32 bitsPerSample, AudioChannels channels) noexcept;

/// Calculates the duration of an audio clip from the sample count and sample rate.
/// @param sampleCount The number of samples per channel.
/// @param sampleRate The number of samples per second (e.g. 44100).
/// @return The duration as a floating-point seconds value.
[[nodiscard]] POMDOG_EXPORT Duration
calculateSampleDuration(i32 sampleCount, i32 sampleRate) noexcept;

} // namespace pomdog::detail
