// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "AudioClipXAudio2.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace XAudio2 {
//-----------------------------------------------------------------------
AudioClipXAudio2::AudioClipXAudio2(std::vector<std::uint8_t> && audioDataIn,
    std::vector<std::uint8_t> && waveFormatIn)
    : audioData(std::move(audioDataIn))
    , waveFormat(std::move(waveFormatIn))
{
    POMDOG_ASSERT(!audioData.empty());
    POMDOG_ASSERT(!waveFormat.empty());
}
//-----------------------------------------------------------------------
WAVEFORMATEX const* AudioClipXAudio2::WaveFormat() const
{
    POMDOG_ASSERT(!waveFormat.empty());
    POMDOG_ASSERT(waveFormat.size() >= sizeof(WAVEFORMATEX));
    return reinterpret_cast<WAVEFORMATEX const*>(waveFormat.data());
}
//-----------------------------------------------------------------------
std::uint8_t const* AudioClipXAudio2::Data() const
{
    POMDOG_ASSERT(!audioData.empty());
    return audioData.data();
}
//-----------------------------------------------------------------------
std::size_t AudioClipXAudio2::SizeInBytes() const
{
    POMDOG_ASSERT(!audioData.empty());
    return sizeof(std::uint8_t) * audioData.size();
}
//-----------------------------------------------------------------------
} // namespace XAudio2
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog
