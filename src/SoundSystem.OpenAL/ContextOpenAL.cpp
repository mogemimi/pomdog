// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "ContextOpenAL.hpp"
#include "ErrorCheckerAL.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace OpenAL {

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

} // namespace OpenAL
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog
