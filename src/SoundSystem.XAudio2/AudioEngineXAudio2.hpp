// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesXAudio2.hpp"
#include <wrl/client.h>

namespace Pomdog::Detail::SoundSystem::XAudio2 {

class AudioEngineXAudio2 final {
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

} // namespace Pomdog::Detail::SoundSystem::XAudio2
