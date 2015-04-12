// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CONTEXTOPENAL_79017363_HPP
#define POMDOG_CONTEXTOPENAL_79017363_HPP

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

} // namespace OpenAL
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_CONTEXTOPENAL_79017363_HPP
