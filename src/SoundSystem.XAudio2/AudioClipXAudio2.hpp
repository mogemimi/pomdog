// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesXAudio2.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace Pomdog {

enum class AudioChannels : std::uint8_t;

namespace Detail {
namespace SoundSystem {
namespace XAudio2 {

class AudioClipXAudio2 final {
public:
    AudioClipXAudio2(
        std::vector<std::uint8_t> && audioData,
        std::vector<std::uint8_t> && waveFormat);

    AudioClipXAudio2(const AudioClipXAudio2&) = delete;
    AudioClipXAudio2& operator=(const AudioClipXAudio2&) = delete;

    const WAVEFORMATEX* WaveFormat() const;

    const std::uint8_t* Data() const;

    std::size_t SizeInBytes() const;

private:
    std::vector<std::uint8_t> audioData;
    std::vector<std::uint8_t> waveFormat;
};

} // namespace XAudio2
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog
