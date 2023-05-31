// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/openal/audio_engine_al.h"
#include "pomdog/audio/openal/audio_clip_al.h"
#include "pomdog/audio/openal/error_checker_al.h"
#include "pomdog/audio/openal/sound_effect_al.h"
#include "pomdog/utility/assert.h"
#include <utility>

namespace pomdog::detail::openal {

AudioEngineAL::AudioEngineAL() noexcept = default;

AudioEngineAL::~AudioEngineAL() noexcept
{
    if (const auto currentContext = alcGetCurrentContext(); currentContext == context_) {
        alcMakeContextCurrent(nullptr);
    }

    if (context_ != nullptr) {
        alcDestroyContext(context_);
        context_ = nullptr;
    }

    if (device_ != nullptr) {
        alcCloseDevice(device_);
        device_ = nullptr;
    }
}

std::unique_ptr<Error>
AudioEngineAL::initialize() noexcept
{
    // NOTE: Select the preferred device.
    device_ = alcOpenDevice(nullptr);

    if (device_ == nullptr) {
        // NOTE: Do not use alGetError() before initializing the OpenAL context.
        return errors::make("alcOpenDevice() failed");
    }

    POMDOG_ASSERT(device_ != nullptr);
    context_ = alcCreateContext(device_, nullptr);
    if (context_ == nullptr) {
        return errors::make("alcCreateContext() failed");
    }

    alcMakeContextCurrent(context_);
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return MakeOpenALError(std::move(err), "alcMakeContextCurrent() failed.");
    }

    return nullptr;
}

std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
AudioEngineAL::createAudioClip(
    const void* audioData,
    std::size_t sizeInBytes,
    int sampleRate,
    int bitsPerSample,
    AudioChannels channels) noexcept
{
    auto audioClip = std::make_shared<AudioClipAL>();

    if (auto err = audioClip->initialize(audioData, sizeInBytes, sampleRate, bitsPerSample, channels); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize AudioClipXAudio2"));
    }

    return std::make_tuple(std::move(audioClip), nullptr);
}

std::tuple<std::shared_ptr<SoundEffect>, std::unique_ptr<Error>>
AudioEngineAL::createSoundEffect(
    const std::shared_ptr<AudioClip>& audioClip,
    bool isLooped) noexcept
{
    POMDOG_ASSERT(audioClip != nullptr);
    POMDOG_ASSERT(std::dynamic_pointer_cast<AudioClipAL>(audioClip) != nullptr);

    auto nativeAudioClip = std::static_pointer_cast<AudioClipAL>(audioClip);
    auto soundEffect = std::make_shared<SoundEffectAL>();

    if (auto err = soundEffect->initialize(nativeAudioClip, isLooped); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize SoundEffectXAudio2"));
    }

    return std::make_tuple(std::move(soundEffect), nullptr);
}

float AudioEngineAL::getMainVolume() const noexcept
{
    float volume = 0.0f;
    alGetListenerf(AL_GAIN, &volume);
    POMDOG_CHECK_ERROR_OPENAL("alGetListenerf(AL_GAIN)");
    return volume;
}

void AudioEngineAL::setMainVolume(float volume) noexcept
{
    POMDOG_ASSERT(volume >= 0.0f);
    alListenerf(AL_GAIN, volume);
    POMDOG_CHECK_ERROR_OPENAL("alListenerf(AL_GAIN)");
}

} // namespace pomdog::detail::openal
