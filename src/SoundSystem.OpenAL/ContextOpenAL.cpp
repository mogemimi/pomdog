//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "ContextOpenAL.hpp"
#include "ErrorCheckerAL.hpp"
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {
namespace Details {
namespace SoundSystem {
namespace OpenAL {
//-----------------------------------------------------------------------
ContextOpenAL::ContextOpenAL()
	: device(nullptr)
	, context(nullptr)
{
	device = alcOpenDevice(nullptr);

	#ifdef DEBUG
	ErrorCheckerAL::CheckError("alcOpenDevice", __FILE__, __LINE__);
	#endif

	POMDOG_ASSERT(device);
	context = alcCreateContext(device, nullptr);
	alcMakeContextCurrent(context);

	#ifdef DEBUG
	ErrorCheckerAL::CheckError("alcMakeContextCurrent", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
ContextOpenAL::~ContextOpenAL()
{
	auto currentContext = alcGetCurrentContext();
	if (currentContext == context) {
		alcMakeContextCurrent(nullptr);
	}

	if (context) {
		alcDestroyContext(context);
	}
	
	if (device) {
		alcCloseDevice(device);
	}
}
//-----------------------------------------------------------------------
}// namespace OpenAL
}// namespace SoundSystem
}// namespace Details
}// namespace Pomdog
