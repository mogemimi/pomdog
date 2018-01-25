// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesOpenAL.hpp"
#include "../Utility/Noncopyable.hpp"

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace OpenAL {

class ContextOpenAL final : Noncopyable {
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
