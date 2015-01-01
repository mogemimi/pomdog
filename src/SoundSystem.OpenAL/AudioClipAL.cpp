//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "AudioClipAL.hpp"
#include "ErrorCheckerAL.hpp"
#include <Pomdog/Audio/AudioClip.hpp>
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
AudioClipAL::AudioClipAL(void const* data, std::size_t size,
	std::uint32_t sampleRate, std::uint16_t bitsPerSample, AudioChannels channel)
	: sizeInBytes(size)
{
	POMDOG_ASSERT(bitsPerSample == 8 || bitsPerSample == 16);

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
AudioClipAL::~AudioClipAL()
{
	if (buffer) {
		alDeleteBuffers(1, &(*buffer));
	}
}
//-----------------------------------------------------------------------
std::size_t AudioClipAL::SizeInBytes() const
{
	return sizeInBytes;
}
//-----------------------------------------------------------------------
ALuint AudioClipAL::NativeBuffer() const
{
	POMDOG_ASSERT(buffer);
	return *buffer;
}
//-----------------------------------------------------------------------
}// namespace OpenAL
}// namespace SoundSystem
}// namespace Details
}// namespace Pomdog
