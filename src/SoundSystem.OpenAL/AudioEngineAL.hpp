// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "ContextOpenAL.hpp"
#include "PrerequisitesOpenAL.hpp"

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace OpenAL {

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

} // namespace OpenAL
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog
