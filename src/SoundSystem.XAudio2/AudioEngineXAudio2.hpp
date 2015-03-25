// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_AUDIOENGINEXAUDIO2_349E822E_8912_46FC_B085_7F3B02738901_HPP
#define POMDOG_AUDIOENGINEXAUDIO2_349E822E_8912_46FC_B085_7F3B02738901_HPP

#if _MSC_VER > 1000
#pragma once
#endif

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

    float MasterVolume() const;

    void MasterVolume(float volume);

private:
    Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
    IXAudio2MasteringVoice* masteringVoice;
};

}// namespace XAudio2
}// namespace SoundSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_AUDIOENGINEXAUDIO2_349E822E_8912_46FC_B085_7F3B02738901_HPP)
