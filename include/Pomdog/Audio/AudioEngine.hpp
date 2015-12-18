// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();

    AudioEngine(AudioEngine const&) = delete;
    AudioEngine & operator=(AudioEngine const&) = delete;
    AudioEngine(AudioEngine &&) = default;
    AudioEngine & operator=(AudioEngine &&) = default;

    float GetMasterVolume() const;

    void SetMasterVolume(float volume);

    Detail::SoundSystem::NativeAudioEngine* NativeAudioEngine();

private:
    std::unique_ptr<Detail::SoundSystem::NativeAudioEngine> nativeAudioEngine;
};

} // namespace Pomdog
