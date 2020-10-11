// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "AudioEngineAL.hpp"
#include "AudioClipAL.hpp"
#include "ErrorCheckerAL.hpp"
#include "SoundEffectAL.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <utility>

namespace Pomdog::Detail::OpenAL {

AudioEngineAL::AudioEngineAL() noexcept = default;

AudioEngineAL::~AudioEngineAL() noexcept
{
    auto currentContext = alcGetCurrentContext();
    if (currentContext == this->context) {
        alcMakeContextCurrent(nullptr);
    }

    if (this->context) {
        alcDestroyContext(this->context);
        this->context = nullptr;
    }

    if (this->device) {
        alcCloseDevice(this->device);
        this->device = nullptr;
    }
}

std::shared_ptr<Error>
AudioEngineAL::Initialize() noexcept
{
    this->device = alcOpenDevice(nullptr);

    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return MakeOpenALError(std::move(err), "alcOpenDevice() failed.");
    }

    POMDOG_ASSERT(device != nullptr);
    this->context = alcCreateContext(this->device, nullptr);
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return MakeOpenALError(std::move(err), "alcCreateContext() failed.");
    }

    alcMakeContextCurrent(this->context);
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return MakeOpenALError(std::move(err), "alcMakeContextCurrent() failed.");
    }

    return nullptr;
}

std::tuple<std::shared_ptr<AudioClip>, std::shared_ptr<Error>>
AudioEngineAL::CreateAudioClip(
    const void* audioData,
    std::size_t sizeInBytes,
    int sampleRate,
    int bitsPerSample,
    AudioChannels channels) noexcept
{
    auto audioClip = std::make_shared<AudioClipAL>();

    if (auto err = audioClip->Initialize(audioData, sizeInBytes, sampleRate, bitsPerSample, channels); err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize AudioClipXAudio2"));
    }

    return std::make_tuple(std::move(audioClip), nullptr);
}

std::tuple<std::shared_ptr<SoundEffect>, std::shared_ptr<Error>>
AudioEngineAL::CreateSoundEffect(
    const std::shared_ptr<AudioClip>& audioClip,
    bool isLooped) noexcept
{
    POMDOG_ASSERT(audioClip != nullptr);
    POMDOG_ASSERT(std::dynamic_pointer_cast<AudioClipAL>(audioClip) != nullptr);

    auto nativeAudioClip = std::static_pointer_cast<AudioClipAL>(audioClip);
    auto soundEffect = std::make_shared<SoundEffectAL>();

    if (auto err = soundEffect->Initialize(nativeAudioClip, isLooped); err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize SoundEffectXAudio2"));
    }

    return std::make_tuple(std::move(soundEffect), nullptr);
}

float AudioEngineAL::GetMainVolume() const noexcept
{
    float volume = 0.0f;
    alGetListenerf(AL_GAIN, &volume);
    POMDOG_CHECK_ERROR_OPENAL("alGetListenerf(AL_GAIN)");
    return volume;
}

void AudioEngineAL::SetMainVolume(float volume) noexcept
{
    POMDOG_ASSERT(volume >= 0.0f);
    alListenerf(AL_GAIN, volume);
    POMDOG_CHECK_ERROR_OPENAL("alListenerf(AL_GAIN)");
}

} // namespace Pomdog::Detail::OpenAL
