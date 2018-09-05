// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesOpenAL.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>

namespace Pomdog {

enum class AudioChannels : std::uint8_t;

namespace Detail {
namespace SoundSystem {
namespace OpenAL {

class AudioClipAL final {
public:
    AudioClipAL(
        const void* data,
        std::size_t size,
        int sampleRate,
        int bitsPerSample,
        AudioChannels channel);

    AudioClipAL(const AudioClipAL&) = delete;
    AudioClipAL& operator=(const AudioClipAL&) = delete;

    ~AudioClipAL();

    std::size_t SizeInBytes() const;

    ALuint NativeBuffer() const;

private:
    std::optional<ALuint> buffer;
    std::size_t sizeInBytes;
};

} // namespace OpenAL
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog
