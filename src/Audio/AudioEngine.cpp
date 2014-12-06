//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Pomdog/Audio/AudioEngine.hpp"

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include "../SoundSystem.OpenAL/AudioEngineAL.hpp"
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "../SoundSystem.XAudio2/AudioEngineXAudio2.hpp"
#endif

#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
AudioEngine::AudioEngine()
	: nativeAudioEngine(std::make_unique<Details::SoundSystem::NativeAudioEngine>())
{
}
//-----------------------------------------------------------------------
AudioEngine::~AudioEngine() = default;
//-----------------------------------------------------------------------
//float AudioEngine::MasterVolume() const
//{
//}
//-----------------------------------------------------------------------
//void AudioEngine::MasterVolume(float volume)
//{
//}
//-----------------------------------------------------------------------
Details::SoundSystem::NativeAudioEngine* AudioEngine::NativeAudioEngine()
{
	POMDOG_ASSERT(nativeAudioEngine);
	return nativeAudioEngine.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
