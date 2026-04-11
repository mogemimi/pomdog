// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/openal/audio_engine_al.h"
#include "pomdog/audio/details/audio_clip_file.h"
#include "pomdog/audio/openal/audio_clip_al.h"
#include "pomdog/audio/openal/audio_clip_streaming_al.h"
#include "pomdog/audio/openal/audio_source_al.h"
#include "pomdog/audio/openal/audio_source_streaming_al.h"
#include "pomdog/audio/openal/error_checker_al.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/static_down_cast.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstring>
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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

    if (alcMakeContextCurrent(context_) == ALC_FALSE) {
        auto alcErr = alcGetError(device_);
        return errors::make("alcMakeContextCurrent() failed: " + std::to_string(alcErr));
    }

    // NOTE: 4KiB audio temporary buffer for streaming
    constexpr u32 tempBufferSize = 1024 * 4;
    streamBuffer_.clear();
    streamBuffer_.resize(tempBufferSize);
    streamSources_.clear();

    return nullptr;
}

void AudioEngineAL::makeCurrentContext() noexcept
{
    POMDOG_ASSERT(context_ != nullptr);
    alcMakeContextCurrent(context_);
}

void AudioEngineAL::clearCurrentContext() noexcept
{
    if (const auto currentContext = alcGetCurrentContext(); currentContext == context_) {
        alcMakeContextCurrent(nullptr);
    }
}

void AudioEngineAL::update() noexcept
{
    bool needRemove = false;

    for (auto& source : streamSources_) {
        auto p = source.lock();
        if (p == nullptr) {
            source.reset();
            needRemove = true;
            continue;
        }
        if (auto err = p->poll(streamBuffer_); err != nullptr) {
            source.reset();
            needRemove = true;
            continue;
        }
    }

    if (needRemove) {
        streamSources_.erase(
            std::remove_if(streamSources_.begin(), streamSources_.end(), [](const auto& p) -> bool {
                return p.expired();
            }),
            streamSources_.end());
    }
}

std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
AudioEngineAL::createAudioClip(
    std::span<const u8> waveData,
    i32 sampleRate,
    i32 bitsPerSample,
    AudioChannels channels) noexcept
{
    auto audioClip = std::make_shared<AudioClipAL>();
    if (auto err = audioClip->initialize(waveData, sampleRate, bitsPerSample, channels); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize AudioClipAL"));
    }
    return std::make_tuple(std::move(audioClip), nullptr);
}

std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
AudioEngineAL::createAudioClip(
    std::unique_ptr<detail::AudioClipFile> audioClipFile) noexcept
{
    auto audioClip = std::make_shared<AudioClipStreamingAL>();
    if (auto err = audioClip->initialize(std::move(audioClipFile)); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize AudioClipStreamingAL"));
    }
    return std::make_tuple(std::move(audioClip), nullptr);
}

std::tuple<std::shared_ptr<AudioSource>, std::unique_ptr<Error>>
AudioEngineAL::createAudioSource(
    const std::shared_ptr<AudioClip>& audioClip,
    bool isLooped) noexcept
{
    POMDOG_ASSERT(audioClip != nullptr);

    if (audioClip->isStreamable()) {
        auto clip = pomdog::detail::static_down_cast<AudioClipStreamingAL>(
            std::const_pointer_cast<AudioClip>(audioClip));

        auto audioSource = std::make_shared<AudioSourceStreamingAL>();
        if (auto err = audioSource->initialize(std::move(clip), streamBuffer_, isLooped); err != nullptr) {
            return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize AudioSourceStreamingAL"));
        }

        streamSources_.push_back(audioSource);

        return std::make_tuple(std::move(audioSource), nullptr);
    }

    auto clip = pomdog::detail::static_down_cast<AudioClipAL>(
        std::const_pointer_cast<AudioClip>(audioClip));

    auto audioSource = std::make_shared<AudioSourceAL>();
    if (auto err = audioSource->initialize(std::move(clip), isLooped); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize AudioSourceAL"));
    }
    return std::make_tuple(std::move(audioSource), nullptr);
}

f32 AudioEngineAL::getMainVolume() const noexcept
{
    f32 volume = 0.0f;
    alGetListenerf(AL_GAIN, &volume);
    POMDOG_CHECK_ERROR_OPENAL("alGetListenerf(AL_GAIN)");
    return volume;
}

void AudioEngineAL::setMainVolume(f32 volume) noexcept
{
    POMDOG_ASSERT(volume >= 0.0f);
    alListenerf(AL_GAIN, volume);
    POMDOG_CHECK_ERROR_OPENAL("alListenerf(AL_GAIN)");
}

} // namespace pomdog::detail::openal
