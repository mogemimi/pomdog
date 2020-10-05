// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Audio/AudioEngine.hpp"

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include "../SoundSystem.OpenAL/AudioEngineAL.hpp"
#elif defined(POMDOG_PLATFORM_LINUX)
#include "../SoundSystem.OpenAL/AudioEngineAL.hpp"
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "../SoundSystem.XAudio2/AudioEngineXAudio2.hpp"
#endif

#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {

AudioEngine::AudioEngine()
    : nativeAudioEngine(std::make_unique<Detail::SoundSystem::NativeAudioEngine>())
{
}

AudioEngine::~AudioEngine() = default;

float AudioEngine::GetMainVolume() const
{
    POMDOG_ASSERT(nativeAudioEngine);
    return nativeAudioEngine->GetMainVolume();
}

void AudioEngine::SetMainVolume(float volume)
{
    POMDOG_ASSERT(volume >= 0);
    POMDOG_ASSERT(nativeAudioEngine);
    nativeAudioEngine->SetMainVolume(volume);
}

Detail::SoundSystem::NativeAudioEngine* AudioEngine::GetNativeAudioEngine()
{
    POMDOG_ASSERT(nativeAudioEngine);
    return nativeAudioEngine.get();
}

} // namespace Pomdog
