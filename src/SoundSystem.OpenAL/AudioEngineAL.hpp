// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "ContextOpenAL.hpp"
#include "PrerequisitesOpenAL.hpp"

namespace Pomdog::Detail::SoundSystem::OpenAL {

class AudioEngineAL final {
public:
    AudioEngineAL();
    AudioEngineAL(const AudioEngineAL&) = delete;
    AudioEngineAL& operator=(const AudioEngineAL&) = delete;
    ~AudioEngineAL();

    float GetMasterVolume() const;

    void SetMasterVolume(float volume);

private:
    ContextOpenAL context;
};

} // namespace Pomdog::Detail::SoundSystem::OpenAL
