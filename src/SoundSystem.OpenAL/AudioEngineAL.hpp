// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesOpenAL.hpp"
#include "ContextOpenAL.hpp"
#include "../Utility/Noncopyable.hpp"

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace OpenAL {

class AudioEngineAL final : Noncopyable {
public:
    AudioEngineAL();
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
