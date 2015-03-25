// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SOUNDEFFECTXAUDIO2_ED1F6835_CC69_4922_802F_961D8011F154_HPP
#define POMDOG_SOUNDEFFECTXAUDIO2_ED1F6835_CC69_4922_802F_961D8011F154_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "PrerequisitesXAudio2.hpp"
#include "../Utility/Noncopyable.hpp"
#include <memory>

namespace Pomdog {
class AudioEmitter;
class AudioListener;
}// namespace Pomdog

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace XAudio2 {

class AudioClipXAudio2;
class AudioEngineXAudio2;

class SoundEffectXAudio2 final: Noncopyable {
public:
	SoundEffectXAudio2(AudioEngineXAudio2 & audioEngine,
		std::shared_ptr<AudioClipXAudio2> const& audioClip, bool isLooped);

	~SoundEffectXAudio2();

	void ExitLoop();

	void Pause();

	void Play();

	void Stop();

	void Apply3D(AudioListener const& listener, AudioEmitter const& emitter);

	void Pitch(float pitch);

	void Volume(float volume);

private:
	std::shared_ptr<AudioClipXAudio2> audioClip;
	IXAudio2SourceVoice* sourceVoice;
};

}// namespace XAudio2
}// namespace SoundSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_SOUNDEFFECTXAUDIO2_ED1F6835_CC69_4922_802F_961D8011F154_HPP)
