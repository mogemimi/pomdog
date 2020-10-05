// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Platform.hpp"
#include <cstdint>

namespace Pomdog {
namespace Detail::SoundSystem {

#if defined(POMDOG_PLATFORM_MACOSX) \
    || defined(POMDOG_PLATFORM_APPLE_IOS) \
    || defined(POMDOG_PLATFORM_LINUX)

namespace OpenAL {

class AudioClipAL;
class AudioEngineAL;
class SoundEffectAL;

} // namespace OpenAL

using NativeAudioClip = OpenAL::AudioClipAL;
using NativeAudioEngine = OpenAL::AudioEngineAL;
using NativeSoundEffect = OpenAL::SoundEffectAL;

#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)

namespace XAudio2 {

class AudioClipXAudio2;
class AudioEngineXAudio2;
class SoundEffectXAudio2;

} // namespace XAudio2

using NativeAudioClip = XAudio2::AudioClipXAudio2;
using NativeAudioEngine = XAudio2::AudioEngineXAudio2;
using NativeSoundEffect = XAudio2::SoundEffectXAudio2;

#else
#error "Platform undefined or not supported."
#endif

} // namespace Detail::SoundSystem

class AudioClip;
class AudioEmitter;
class AudioEngine;
class AudioListener;
class SoundEffect;

enum class AudioChannel : std::uint8_t;
enum class SoundState : std::uint8_t;

} // namespace Pomdog
