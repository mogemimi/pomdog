//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_FORWARDDECLARATIONS_0BAC0396_5240_4F14_AFF8_13A71B8E0B54_HPP
#define POMDOG_FORWARDDECLARATIONS_0BAC0396_5240_4F14_AFF8_13A71B8E0B54_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Basic/Platform.hpp"

namespace Pomdog {
namespace Details {
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
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_FORWARDDECLARATIONS_0BAC0396_5240_4F14_AFF8_13A71B8E0B54_HPP)
