//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SoundEffectAL.hpp"
#include "AudioBufferAL.hpp"
#include "ErrorCheckerAL.hpp"
#include <Pomdog/Audio/AudioEmitter.hpp>
#include <Pomdog/Audio/AudioListener.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <array>

namespace Pomdog {
namespace Details {
namespace SoundSystem {
namespace OpenAL {
//-----------------------------------------------------------------------
SoundEffectAL::SoundEffectAL(std::shared_ptr<AudioBufferAL> const& audioBufferIn)
	: audioBuffer(audioBufferIn)
{
	source = ([] {
		ALuint nativeSource;
		alGenSources(1, &nativeSource);
		return std::move(nativeSource);
	})();

	#ifdef DEBUG
	ErrorCheckerAL::CheckError("alGenSources", __FILE__, __LINE__);
	#endif
		
	POMDOG_ASSERT(source);
	alSourcei(source->value, AL_BUFFER, audioBuffer->NativeBuffer());
	
	#ifdef DEBUG
	ErrorCheckerAL::CheckError("alSourcei", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
SoundEffectAL::~SoundEffectAL()
{
	if (source) {
		alDeleteSources(1, source->Data());
	}
}
//-----------------------------------------------------------------------
void SoundEffectAL::Pause()
{
	POMDOG_ASSERT(source);
	alSourcePause(source->value);
	
	#ifdef DEBUG
	ErrorCheckerAL::CheckError("alSourcePause", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void SoundEffectAL::Play()
{
	POMDOG_ASSERT(source);
	alSourcePlay(source->value);
	
	#ifdef DEBUG
	ErrorCheckerAL::CheckError("alSourcePlay", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void SoundEffectAL::Stop()
{
	POMDOG_ASSERT(source);
	alSourceStop(source->value);
	
	#ifdef DEBUG
	ErrorCheckerAL::CheckError("alSourceStop", __FILE__, __LINE__);
	#endif
	
	POMDOG_ASSERT(source);
	alSourceRewind(source->value);
	
	#ifdef DEBUG
	ErrorCheckerAL::CheckError("alSourceRewind", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void SoundEffectAL::Apply3D(AudioListener const& listener, AudioEmitter const& emitter)
{
	{
		POMDOG_ASSERT(source);
		alSourcefv(source->value, AL_POSITION, emitter.Position.Data());
		alSourcefv(source->value, AL_VELOCITY, emitter.Velocity.Data());
		alSourcefv(source->value, AL_DIRECTION, emitter.Forward.Data());
		
		#ifdef DEBUG
		ErrorCheckerAL::CheckError("alSourcei", __FILE__, __LINE__);
		#endif
		
		POMDOG_ASSERT(emitter.DopplerScale >= 0.0f);
		alDopplerFactor(emitter.DopplerScale);
		
		#ifdef DEBUG
		ErrorCheckerAL::CheckError("alDopplerFactor", __FILE__, __LINE__);
		#endif
	}
	{
		std::array<ALfloat, 6> orientation {{
			listener.Forward.X, listener.Forward.Y, listener.Forward.Z,
			listener.Up.X, listener.Up.Y, listener.Up.Z}};
		
		alListenerfv(AL_POSITION, listener.Position.Data());
		alListenerfv(AL_VELOCITY, listener.Velocity.Data());
		alListenerfv(AL_ORIENTATION, orientation.data());
		
		#ifdef DEBUG
		ErrorCheckerAL::CheckError("alListenerfv", __FILE__, __LINE__);
		#endif
	}
}
//-----------------------------------------------------------------------
void SoundEffectAL::IsLooped(bool isLooped)
{
	POMDOG_ASSERT(source);
	alSourcei(source->value, AL_LOOPING, AL_TRUE);

	#ifdef DEBUG
	ErrorCheckerAL::CheckError("alSourcei", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void SoundEffectAL::Pitch(float pitch)
{
	// pitch --> nativePitch
	//  0.0f --> 1.0f
	// -1.0f --> 0.5f
	// +1.0f --> 1.5f
	auto nativePitch = 1.0f + (0.5f * pitch);
	
	POMDOG_ASSERT(source);
	POMDOG_ASSERT(nativePitch > 0.0f);
	alSourcef(source->value, AL_PITCH, nativePitch);

	#ifdef DEBUG
	ErrorCheckerAL::CheckError("alSourcei", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void SoundEffectAL::Volume(float volume)
{
	POMDOG_ASSERT(source);
	POMDOG_ASSERT(volume >= 0.0f);
	alSourcef(source->value, AL_GAIN, volume);
	
	#ifdef DEBUG
	ErrorCheckerAL::CheckError("alSourcei", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
}// namespace OpenAL
}// namespace SoundSystem
}// namespace Details
}// namespace Pomdog
