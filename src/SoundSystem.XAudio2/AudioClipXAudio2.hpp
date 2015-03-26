// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_AUDIOCLIPXAUDIO2_26CA9C69_BA0C_455B_80AA_CC85E274D3A2_HPP
#define POMDOG_AUDIOCLIPXAUDIO2_26CA9C69_BA0C_455B_80AA_CC85E274D3A2_HPP

#include "PrerequisitesXAudio2.hpp"
#include "../Utility/Noncopyable.hpp"
#include <cstdint>
#include <cstddef>
#include <vector>

namespace Pomdog {

enum class AudioChannels : std::uint8_t;

namespace Detail {
namespace SoundSystem {
namespace XAudio2 {

class AudioClipXAudio2 final: Noncopyable {
public:
    AudioClipXAudio2(std::vector<std::uint8_t> && audioData,
        std::vector<std::uint8_t> && waveFormat);

    WAVEFORMATEX const* WaveFormat() const;

    std::uint8_t const* Data() const;

    std::size_t SizeInBytes() const;

private:
    std::vector<std::uint8_t> audioData;
    std::vector<std::uint8_t> waveFormat;
};

}// namespace XAudio2
}// namespace SoundSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_AUDIOCLIPXAUDIO2_26CA9C69_BA0C_455B_80AA_CC85E274D3A2_HPP)
