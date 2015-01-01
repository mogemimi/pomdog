//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "AudioEngineAL.hpp"
#include "ErrorCheckerAL.hpp"
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
namespace Details {
namespace SoundSystem {
namespace OpenAL {
//-----------------------------------------------------------------------
AudioEngineAL::AudioEngineAL()
{
}
//-----------------------------------------------------------------------
AudioEngineAL::~AudioEngineAL() = default;
//-----------------------------------------------------------------------
float AudioEngineAL::MasterVolume() const
{
	float volume = 0.0f;
	alGetListenerf(AL_GAIN, &volume);
	return volume;
}
//-----------------------------------------------------------------------
void AudioEngineAL::MasterVolume(float volume)
{
	POMDOG_ASSERT(volume >= 0.0f);
	alListenerf(AL_GAIN, volume);
}
//-----------------------------------------------------------------------
}// namespace OpenAL
}// namespace SoundSystem
}// namespace Details
}// namespace Pomdog
