// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#include "AudioEngineAL.hpp"
#include "ErrorCheckerAL.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace OpenAL {
//-----------------------------------------------------------------------
AudioEngineAL::AudioEngineAL()
{
}
//-----------------------------------------------------------------------
AudioEngineAL::~AudioEngineAL() = default;
//-----------------------------------------------------------------------
float AudioEngineAL::GetMasterVolume() const
{
    float volume = 0.0f;
    alGetListenerf(AL_GAIN, &volume);
    return volume;
}
//-----------------------------------------------------------------------
void AudioEngineAL::SetMasterVolume(float volume)
{
    POMDOG_ASSERT(volume >= 0.0f);
    alListenerf(AL_GAIN, volume);
}
//-----------------------------------------------------------------------
} // namespace OpenAL
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog
