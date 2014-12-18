//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_CONTEXTOPENAL_79017363_0C9F_44B8_B79F_C3D84CBB340C_HPP
#define POMDOG_CONTEXTOPENAL_79017363_0C9F_44B8_B79F_C3D84CBB340C_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "PrerequisitesOpenAL.hpp"
#include "../Utility/Noncopyable.hpp"

namespace Pomdog {
namespace Details {
namespace SoundSystem {
namespace OpenAL {

class ContextOpenAL final: Noncopyable {
private:
	ALCdevice* device;
	ALCcontext* context;

public:
	ContextOpenAL();
	~ContextOpenAL();
};

}// namespace OpenAL
}// namespace SoundSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_CONTEXTOPENAL_79017363_0C9F_44B8_B79F_C3D84CBB340C_HPP)
