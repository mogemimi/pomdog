// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/audio_helper.h"
#include "pomdog/audio/audio_channels.h"
#include "pomdog/utility/assert.h"

namespace pomdog::detail::AudioHelper {

[[nodiscard]] std::size_t
getSamples(std::size_t sizeInBytes, int bitsPerSample, AudioChannels channels) noexcept
{
    POMDOG_ASSERT(bitsPerSample >= 8);
    POMDOG_ASSERT(bitsPerSample == 8 || bitsPerSample == 16 || bitsPerSample == 24 || bitsPerSample == 32);

    static_assert(static_cast<int>(AudioChannels::Mono) == 1, "");
    static_assert(static_cast<int>(AudioChannels::Stereo) == 2, "");

    auto channelCount = static_cast<int>(channels);
    POMDOG_ASSERT(channelCount > 0);
    POMDOG_ASSERT(channelCount <= 2);

    auto divisior = (bitsPerSample / 8) * channelCount;
    POMDOG_ASSERT(divisior > 0);

    return sizeInBytes / static_cast<std::size_t>(divisior);
}

[[nodiscard]] Duration
getSampleDuration(std::size_t samples, int sampleRate) noexcept
{
    POMDOG_ASSERT(sampleRate > 0);
    POMDOG_ASSERT(sampleRate >= 8000);
    POMDOG_ASSERT(sampleRate <= 48000);
    return std::chrono::seconds(samples / static_cast<std::size_t>(sampleRate));
}

} // namespace pomdog::detail::AudioHelper
