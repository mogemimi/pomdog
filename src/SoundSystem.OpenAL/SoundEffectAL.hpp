//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SOUNDEFFECTAL_4A967601_7B5B_4A2E_BBF7_3658E1856E6F_HPP
#define POMDOG_SOUNDEFFECTAL_4A967601_7B5B_4A2E_BBF7_3658E1856E6F_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "PrerequisitesOpenAL.hpp"
#include "../Utility/Noncopyable.hpp"
#include <Pomdog/Utility/detail/Tagged.hpp>
#include <Pomdog/Utility/Optional.hpp>
#include <memory>

namespace Pomdog {
class AudioEmitter;
class AudioListener;
}// namespace Pomdog

namespace Pomdog {
namespace Details {
namespace SoundSystem {
namespace OpenAL {

struct AudioSourceALTag {};
using AudioSourceAL = Tagged<ALuint, AudioSourceALTag>;

class AudioBufferAL;
class AudioEngineAL;

class SoundEffectAL final: Noncopyable {
private:
	std::shared_ptr<AudioBufferAL> audioBuffer;
	Optional<AudioSourceAL> source;

public:
	SoundEffectAL(AudioEngineAL & audioEngine,
		std::shared_ptr<AudioBufferAL> const& audioBuffer, bool isLooped);
	
	~SoundEffectAL();

	void Pause();

	void Play();

	void Stop();
	
	void Apply3D(AudioListener const& listener, AudioEmitter const& emitter);

	void ExitLoop();

	void Pitch(float pitch);

	void Volume(float volume);
};

}// namespace OpenAL
}// namespace SoundSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SOUNDEFFECTAL_4A967601_7B5B_4A2E_BBF7_3658E1856E6F_HPP)
