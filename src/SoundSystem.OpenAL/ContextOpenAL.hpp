// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesOpenAL.hpp"

namespace Pomdog::Detail::SoundSystem::OpenAL {

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

} // namespace Pomdog::Detail::SoundSystem::OpenAL
