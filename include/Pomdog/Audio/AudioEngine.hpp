// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Audio/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT AudioEngine final {
public:
    AudioEngine();
    ~AudioEngine();

    AudioEngine(const AudioEngine&) = delete;
    AudioEngine& operator=(const AudioEngine&) = delete;
    AudioEngine(AudioEngine&&) = default;
    AudioEngine& operator=(AudioEngine&&) = default;

    float GetMasterVolume() const;

    void SetMasterVolume(float volume);

    Detail::SoundSystem::NativeAudioEngine* GetNativeAudioEngine();

private:
    std::unique_ptr<Detail::SoundSystem::NativeAudioEngine> nativeAudioEngine;
};

} // namespace Pomdog
