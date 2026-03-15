// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/audio_channels.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <span>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class AudioContainer final {
public:
    /// Byte array of PCM-formatted audio data.
    std::span<const u8> audioData;

    /// Raw audio data.
    std::vector<u8> rawData;

    /// The number of audio samples per second (sample rate).
    i32 sampleRate = 0;

    /// The number of bits used to represent each audio sample.
    i32 bitsPerSample = 0;

    /// The number of audio channels (e.g., Mono or Stereo).
    AudioChannels channels = AudioChannels::Mono;
};

} // namespace pomdog
