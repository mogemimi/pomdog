// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CONTEXTOPENAL_79017363_0C9F_44B8_B79F_C3D84CBB340C_HPP
#define POMDOG_CONTEXTOPENAL_79017363_0C9F_44B8_B79F_C3D84CBB340C_HPP

#include "PrerequisitesOpenAL.hpp"
#include "../Utility/Noncopyable.hpp"

namespace Pomdog {
namespace Detail {
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
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_CONTEXTOPENAL_79017363_0C9F_44B8_B79F_C3D84CBB340C_HPP)
