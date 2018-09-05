// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesOpenAL.hpp"

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace OpenAL {

class ContextOpenAL final {
private:
    ALCdevice* device;
    ALCcontext* context;

public:
    ContextOpenAL();
    ContextOpenAL(const ContextOpenAL&) = delete;
    ContextOpenAL& operator=(const ContextOpenAL&) = delete;
    ~ContextOpenAL();
};

} // namespace OpenAL
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog
