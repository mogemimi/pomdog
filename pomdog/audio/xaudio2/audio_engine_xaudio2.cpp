// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/xaudio2/audio_engine_xaudio2.h"
#include "pomdog/audio/details/audio_clip_file.h"
#include "pomdog/audio/xaudio2/audio_clip_streaming_xaudio2.h"
#include "pomdog/audio/xaudio2/audio_clip_xaudio2.h"
#include "pomdog/audio/xaudio2/audio_source_streaming_xaudio2.h"
#include "pomdog/audio/xaudio2/audio_source_xaudio2.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/static_down_cast.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <chrono>
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::xaudio2 {

AudioEngineXAudio2::AudioEngineXAudio2() noexcept = default;

AudioEngineXAudio2::~AudioEngineXAudio2() noexcept
{
    exitLoop_ = true;
    if (thread_.joinable()) {
        thread_.join();
    }
    thread_ = {};

    if (mainVoice_ != nullptr) {
        mainVoice_->DestroyVoice();
        mainVoice_ = nullptr;
    }

    if (xAudio2_ != nullptr) {
        xAudio2_.Reset();
    }

    if (comInitialized_) {
        ::CoUninitialize();
        comInitialized_ = false;
    }
}

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(5045)
std::unique_ptr<Error>
AudioEngineXAudio2::initialize() noexcept
{
    if (!comInitialized_) {
        if (auto hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED); FAILED(hr)) {
            return errors::make("CoInitializeEx() failed: " + std::to_string(hr));
        }
        comInitialized_ = true;
    }

#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    constexpr UINT32 flags = XAUDIO2_DEBUG_ENGINE;
#else
    constexpr UINT32 flags = 0;
#endif

    if (auto hr = ::XAudio2Create(&xAudio2_, flags, XAUDIO2_DEFAULT_PROCESSOR); FAILED(hr)) {
        return errors::make("XAudio2Create() failed: " + std::to_string(hr));
    }

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    {
        XAUDIO2_DEBUG_CONFIGURATION debugConfig = {};
        debugConfig.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
        debugConfig.BreakMask = 0;
        debugConfig.LogThreadID = FALSE;
        debugConfig.LogFileline = FALSE;
        debugConfig.LogFunctionName = FALSE;
        debugConfig.LogTiming = FALSE;

        POMDOG_ASSERT(xAudio2_ != nullptr);
        xAudio2_->SetDebugConfiguration(&debugConfig, nullptr);
    }
#endif

    // FIXME: Enumerate available audio devices and choose the one.
    wchar_t* deviceID = nullptr;

    if (auto hr = xAudio2_->CreateMasteringVoice(
            &mainVoice_,
            XAUDIO2_DEFAULT_CHANNELS,
            XAUDIO2_DEFAULT_SAMPLERATE,
            0,
            deviceID,
            nullptr,
            AudioCategory_GameEffects);
        FAILED(hr)) {
        xAudio2_.Reset();
        return errors::make("CreateMasteringVoice() failed: " + std::to_string(hr));
    }

    streamSources_.clear();

    thread_ = std::thread{[this] {
        while (!exitLoop_) {
            update();
        }
    }};

    return nullptr;
}
POMDOG_MSVC_SUPPRESS_WARNING_POP

void AudioEngineXAudio2::update() noexcept
{
    {
        std::lock_guard<std::mutex> lock(mutex_);

        bool needRemove = false;

        for (auto& source : streamSources_) {
            if (exitLoop_) {
                break;
            }

            auto p = source.lock();
            if (p == nullptr) {
                source.reset();
                needRemove = true;
                continue;
            }
            if (auto err = p->poll(); err != nullptr) {
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

    std::this_thread::sleep_for(std::chrono::milliseconds(4));
}

std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
AudioEngineXAudio2::createAudioClip(
    std::span<const u8> waveData,
    i32 sampleRate,
    i32 bitsPerSample,
    AudioChannels channels) noexcept
{
    auto audioClip = std::make_shared<AudioClipXAudio2>();
    if (auto err = audioClip->initialize(waveData, sampleRate, bitsPerSample, channels); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize AudioClipXAudio2"));
    }
    return std::make_tuple(std::move(audioClip), nullptr);
}

std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
AudioEngineXAudio2::createAudioClip(
    std::unique_ptr<detail::AudioClipFile> audioClipFile) noexcept
{
    auto audioClip = std::make_shared<AudioClipStreamingXAudio2>();
    if (auto err = audioClip->initialize(std::move(audioClipFile)); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize AudioClipStreamingXAudio2"));
    }
    return std::make_tuple(std::move(audioClip), nullptr);
}

std::tuple<std::shared_ptr<AudioSource>, std::unique_ptr<Error>>
AudioEngineXAudio2::createAudioSource(
    const std::shared_ptr<AudioClip>& audioClip,
    bool isLooped) noexcept
{
    POMDOG_ASSERT(audioClip != nullptr);

    if (audioClip->isStreamable()) {
        auto clip = pomdog::detail::static_down_cast<AudioClipStreamingXAudio2>(
            std::const_pointer_cast<AudioClip>(audioClip));

        auto audioSource = std::make_shared<AudioSourceStreamingXAudio2>();
        if (auto err = audioSource->initialize(xAudio2_.Get(), std::move(clip), isLooped); err != nullptr) {
            return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize AudioSourceStreamingXAudio2"));
        }

        {
            std::lock_guard<std::mutex> lock(mutex_);
            streamSources_.push_back(audioSource);
        }

        return std::make_tuple(std::move(audioSource), nullptr);
    }

    auto clip = pomdog::detail::static_down_cast<AudioClipXAudio2>(
        std::const_pointer_cast<AudioClip>(audioClip));

    auto audioSource = std::make_shared<AudioSourceXAudio2>();
    if (auto err = audioSource->initialize(xAudio2_.Get(), std::move(clip), isLooped); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize AudioSourceXAudio2"));
    }
    return std::make_tuple(std::move(audioSource), nullptr);
}

f32 AudioEngineXAudio2::getMainVolume() const noexcept
{
    f32 volume = 0.0f;
    if (xAudio2_ && (mainVoice_ != nullptr)) {
        mainVoice_->GetVolume(&volume);
    }
    return volume;
}

void AudioEngineXAudio2::setMainVolume(f32 volume) noexcept
{
    POMDOG_ASSERT(volume >= 0.0f);
    if (xAudio2_ && (mainVoice_ != nullptr)) {
        mainVoice_->SetVolume(math::saturate(volume), XAUDIO2_COMMIT_NOW);
    }
}

unsafe_ptr<IXAudio2>
AudioEngineXAudio2::getXAudio2Engine() const noexcept
{
    POMDOG_ASSERT(xAudio2_ != nullptr);
    return xAudio2_.Get();
}

} // namespace pomdog::detail::xaudio2
