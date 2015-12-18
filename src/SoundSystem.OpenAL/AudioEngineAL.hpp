// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_AUDIOENGINEAL_4B9A3A11_HPP
#define POMDOG_AUDIOENGINEAL_4B9A3A11_HPP

#include "PrerequisitesOpenAL.hpp"
#include "ContextOpenAL.hpp"
#include "../Utility/Noncopyable.hpp"

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace OpenAL {

class AudioEngineAL final: Noncopyable {
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

#endif // POMDOG_AUDIOENGINEAL_4B9A3A11_HPP
