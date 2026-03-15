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
enum class AudioChannels : std::uint8_t;
} // namespace pomdog

namespace pomdog::detail {

[[nodiscard]] POMDOG_EXPORT u8
toAudioChannelCount(AudioChannels channels) noexcept;

[[nodiscard]] POMDOG_EXPORT std::optional<AudioChannels>
toAudioChannels(i32 channelCount) noexcept;

[[nodiscard]] POMDOG_EXPORT i32
calculateSampleCount(i32 sizeInBytes, i32 bitsPerSample, AudioChannels channels) noexcept;

[[nodiscard]] POMDOG_EXPORT Duration
calculateSampleDuration(i32 sampleCount, i32 sampleRate) noexcept;

} // namespace pomdog::detail
