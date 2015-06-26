// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_AUDIOENGINEXAUDIO2_349E822E_HPP
#define POMDOG_AUDIOENGINEXAUDIO2_349E822E_HPP

#include "PrerequisitesXAudio2.hpp"
#include <wrl/client.h>

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace XAudio2 {

class AudioEngineXAudio2 {
public:
    AudioEngineXAudio2();

    ~AudioEngineXAudio2();

    IXAudio2* XAudio2Engine() const;

    float GetMasterVolume() const;

    void SetMasterVolume(float volume);

private:
    Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
    IXAudio2MasteringVoice* masteringVoice;
};

} // namespace XAudio2
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_AUDIOENGINEXAUDIO2_349E822E_HPP
