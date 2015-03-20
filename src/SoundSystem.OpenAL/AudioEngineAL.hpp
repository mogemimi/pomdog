//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_AUDIOENGINEAL_4B9A3A11_27A6_4393_A5FF_F10183A42D92_HPP
#define POMDOG_AUDIOENGINEAL_4B9A3A11_27A6_4393_A5FF_F10183A42D92_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "PrerequisitesOpenAL.hpp"
#include "ContextOpenAL.hpp"
#include "../Utility/Noncopyable.hpp"

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace OpenAL {

class AudioEngineAL final: Noncopyable {
public:
	AudioEngineAL();
	~AudioEngineAL();

	float MasterVolume() const;

	void MasterVolume(float volume);

private:
	ContextOpenAL context;
};

}// namespace OpenAL
}// namespace SoundSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_AUDIOENGINEAL_4B9A3A11_27A6_4393_A5FF_F10183A42D92_HPP)
