// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_FORWARDDECLARATIONS_0BAC0396_HPP
#define POMDOG_FORWARDDECLARATIONS_0BAC0396_HPP

#include "Pomdog/Basic/Platform.hpp"

namespace Pomdog {
namespace Detail {
namespace SoundSystem {

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)

namespace OpenAL {

class AudioClipAL;
class AudioEngineAL;
class SoundEffectAL;

}// namespace OpenAL

using NativeAudioClip = OpenAL::AudioClipAL;
using NativeAudioEngine = OpenAL::AudioEngineAL;
using NativeSoundEffect = OpenAL::SoundEffectAL;

#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)

namespace XAudio2 {

class AudioClipXAudio2;
class AudioEngineXAudio2;
class SoundEffectXAudio2;

}// namespace XAudio2

using NativeAudioClip = XAudio2::AudioClipXAudio2;
using NativeAudioEngine = XAudio2::AudioEngineXAudio2;
using NativeSoundEffect = XAudio2::SoundEffectXAudio2;

#endif

}// namespace SoundSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_FORWARDDECLARATIONS_0BAC0396_HPP
