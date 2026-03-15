// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/details/audio_clip_helpers.h"
#include "pomdog/audio/audio_channels.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

[[nodiscard]] u8
toAudioChannelCount(AudioChannels channels) noexcept
{
    switch (channels) {
    case AudioChannels::Mono:
        return 1;
    case AudioChannels::Stereo:
        return 2;
    }
    POMDOG_UNREACHABLE("unsupported audio channels");
}

[[nodiscard]] std::optional<AudioChannels>
toAudioChannels(i32 channelCount) noexcept
{
    switch (channelCount) {
    case 1:
        return AudioChannels::Mono;
    case 2:
        return AudioChannels::Stereo;
    }
    return std::nullopt;
}

[[nodiscard]] i32
calculateSampleCount(i32 sizeInBytes, i32 bitsPerSample, AudioChannels channels) noexcept
{
    POMDOG_ASSERT(bitsPerSample >= 8);
    POMDOG_ASSERT(bitsPerSample == 8 || bitsPerSample == 16 || bitsPerSample == 24 || bitsPerSample == 32);

    const auto channelCount = toAudioChannelCount(channels);
    POMDOG_ASSERT(channelCount > 0);
    POMDOG_ASSERT(channelCount <= 2);

    const auto divisor = (bitsPerSample / 8) * channelCount;
    POMDOG_ASSERT(divisor > 0);

    return sizeInBytes / divisor;
}

[[nodiscard]] Duration
calculateSampleDuration(i32 sampleCount, i32 sampleRate) noexcept
{
    POMDOG_ASSERT(sampleRate > 0);
    POMDOG_ASSERT(sampleRate >= 8000);
    POMDOG_ASSERT(sampleRate <= 48000);
    POMDOG_ASSERT(sampleCount >= 0);
    return Duration{static_cast<f64>(sampleCount) / static_cast<f64>(sampleRate)};
}

} // namespace pomdog::detail
