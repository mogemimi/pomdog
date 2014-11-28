//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AudioBufferAL.hpp"
#include "ErrorCheckerAL.hpp"
#include <Pomdog/Audio/AudioBuffer.hpp>
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
namespace Details {
namespace SoundSystem {
namespace OpenAL {
namespace {

static ALenum ToFormat(AudioChannels channel, std::uint16_t bitPerSample)
{
	switch (channel) {
	case AudioChannels::Mono:
		if (bitPerSample == 8) {
			return AL_FORMAT_MONO8;
		}
		if (bitPerSample == 16) {
			return AL_FORMAT_MONO16;
		}
		break;
	case AudioChannels::Stereo:
		if (bitPerSample == 8) {
			return AL_FORMAT_STEREO8;
		}
		if (bitPerSample == 16) {
			return AL_FORMAT_STEREO16;
		}
		break;
	}
	
	POMDOG_ASSERT(false);
	return AL_FORMAT_MONO8;
}

}// unnamed namespace
//-----------------------------------------------------------------------
AudioBufferAL::AudioBufferAL(void const* data, std::size_t size, AudioChannels channel,
	std::uint16_t bitsPerSample, std::uint32_t sampleRate)
{
	buffer = ([] {
		ALuint nativeBuffer;
		alGenBuffers(1, &nativeBuffer);
		return std::move(nativeBuffer);
	})();
	
	#ifdef DEBUG
	ErrorCheckerAL::CheckError("alGenBuffers", __FILE__, __LINE__);
	#endif
	
	POMDOG_ASSERT(buffer);
	
	POMDOG_ASSERT(data);
	POMDOG_ASSERT(size > 0);
	
	ALenum format = ToFormat(channel, bitsPerSample);
	alBufferData(*buffer, format, data, static_cast<ALsizei>(size), sampleRate);
	
	#ifdef DEBUG
	ErrorCheckerAL::CheckError("alBufferData", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
AudioBufferAL::~AudioBufferAL()
{
	if (buffer) {
		alDeleteBuffers(1, &(*buffer));
	}
}
//-----------------------------------------------------------------------
ALuint AudioBufferAL::NativeBuffer() const
{
	POMDOG_ASSERT(buffer);
	return *buffer;
}
//-----------------------------------------------------------------------
}// namespace OpenAL
}// namespace SoundSystem
}// namespace Details
}// namespace Pomdog
