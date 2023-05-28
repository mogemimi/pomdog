// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/xaudio2/audio_engine_xaudio2.h"
#include "pomdog/audio/xaudio2/audio_clip_xaudio2.h"
#include "pomdog/audio/xaudio2/sound_effect_xaudio2.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/math.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/) && !defined(__cplusplus_winrt)
#include <Windows.Devices.Enumeration.h>
#include <Windows.Foundation.Collections.h>
#include <wrl/client.h>
#include <wrl/event.h>
#include <wrl/wrappers/corewrappers.h>
#pragma comment(lib, "runtimeobject.lib")
#endif

#include <algorithm>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::xaudio2 {
namespace {

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)

struct AudioDeviceDetails final {
    std::wstring DeviceID;
    std::wstring DisplayName;
    bool IsDefault = false;
    bool IsEnabled = false;
};

[[nodiscard]] std::tuple<std::vector<AudioDeviceDetails>, std::unique_ptr<Error>>
EnumerateAudioDevices() noexcept
{
    std::vector<AudioDeviceDetails> result;

#if defined(_XBOX_ONE)
    ///@todo Not implemented
#elif (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(__cplusplus_winrt)
    ///@todo Not implemented

    //using Windows::Devices::Enumeration::DeviceClass;
    //using Windows::Devices::Enumeration::DeviceInformation;
    //using Windows::Devices::Enumeration::DeviceInformationCollection;

    //auto operation = DeviceInformation::FindAllAsync(DeviceClass::AudioRender);
    //while (operation->Status != Windows::Foundation::AsyncStatus::Completed);

    //DeviceInformationCollection^ devices = operation->GetResults();

    //for (unsigned int index = 0; index < devices->Size; ++index) {
    //    using Windows::Devices::Enumeration::DeviceInformation;
    //    DeviceInformation^ deviceInfo = devices->GetAt(index);
    //    //deviceInfo->Name->Data();
    //    //deviceInfo->Id->Data();
    //}

#elif (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/) && !defined(__cplusplus_winrt)

    using Microsoft::WRL::Callback;
    using Microsoft::WRL::ComPtr;
    using namespace Microsoft::WRL::Wrappers;
    using namespace ABI::Windows::Foundation;
    using namespace ABI::Windows::Foundation::Collections;
    using namespace ABI::Windows::Devices::Enumeration;

    RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
    HRESULT hr = initialize;

    if (FAILED(hr)) {
        return std::make_tuple(std::move(result), errors::make("RoInitialize() failed: " + std::to_string(hr)));
    }

    ComPtr<IDeviceInformationStatics> deviceInfomationFactory;
    hr = GetActivationFactory(
        HStringReference(RuntimeClass_Windows_Devices_Enumeration_DeviceInformation).Get(),
        &deviceInfomationFactory);

    if (FAILED(hr)) {
        return std::make_tuple(std::move(result), errors::make("GetActivationFactory() failed: " + std::to_string(hr)));
    }

    Event findCompleted(CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, WRITE_OWNER | EVENT_ALL_ACCESS));

    if (!findCompleted.IsValid()) {
        return std::make_tuple(std::move(result), errors::make("CreateEventEx() failed"));
    }

    ComPtr<IAsyncOperation<DeviceInformationCollection*>> findOperation;
    hr = deviceInfomationFactory->FindAllAsyncDeviceClass(DeviceClass_AudioRender, findOperation.GetAddressOf());

    if (FAILED(hr)) {
        return std::make_tuple(std::move(result), errors::make("FindAllAsyncDeviceClass() failed: " + std::to_string(hr)));
    }

    auto callback = Callback<IAsyncOperationCompletedHandler<DeviceInformationCollection*>>(
        [&findCompleted](IAsyncOperation<DeviceInformationCollection*>*, AsyncStatus) -> HRESULT {
            SetEvent(findCompleted.Get());
            return S_OK;
        });

    findOperation->put_Completed(callback.Get());

    WaitForSingleObjectEx(findCompleted.Get(), INFINITE, FALSE);

    ComPtr<IVectorView<DeviceInformation*>> devices;
    findOperation->GetResults(devices.GetAddressOf());

    unsigned int count = 0;
    hr = devices->get_Size(&count);

    if (FAILED(hr)) {
        return std::make_tuple(std::move(result), errors::make("get_Size() failed: " + std::to_string(hr)));
    }

    if (count <= 0) {
        return std::make_tuple(std::move(result), nullptr);
    }

    POMDOG_ASSERT(count >= 1);
    result.reserve(count);

    for (unsigned int index = 0; index < count; ++index) {
        ComPtr<IDeviceInformation> deviceInfo;
        hr = devices->GetAt(index, deviceInfo.GetAddressOf());

        if (FAILED(hr)) {
            continue;
        }

        HString id;
        deviceInfo->get_Id(id.GetAddressOf());
        HString name;
        deviceInfo->get_Name(name.GetAddressOf());

        AudioDeviceDetails deviceDetails;
        deviceDetails.DeviceID = id.GetRawBuffer(nullptr);
        deviceDetails.DisplayName = name.GetRawBuffer(nullptr);
        deviceDetails.IsDefault = false;
        deviceDetails.IsEnabled = false;

        {
            ::boolean isDefault = FALSE;
            if (SUCCEEDED(deviceInfo->get_IsDefault(&isDefault))) {
                deviceDetails.IsDefault = (isDefault == TRUE);
            }
        }
        {
            ::boolean isEnabled = FALSE;
            if (SUCCEEDED(deviceInfo->get_IsEnabled(&isEnabled))) {
                deviceDetails.IsEnabled = (isEnabled == TRUE);
            }
        }

        result.push_back(std::move(deviceDetails));
    }

    std::stable_sort(std::begin(result), std::end(result), [](const auto& a, const auto& b) {
        int priorityA = (a.IsEnabled ? 100 : 0) + (a.IsDefault ? 10 : 0);
        int priorityB = (b.IsEnabled ? 100 : 0) + (b.IsDefault ? 10 : 0);
        return priorityA > priorityB;
    });

    return std::make_tuple(std::move(result), nullptr);
#endif
}
#endif

} // namespace

AudioEngineXAudio2::AudioEngineXAudio2() noexcept = default;

std::unique_ptr<Error>
AudioEngineXAudio2::Initialize() noexcept
{
    if (auto hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED); FAILED(hr)) {
        return errors::make("CoInitializeEx() failed: " + std::to_string(hr));
    }

#if (_WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(DEBUG) && !defined(NDEBUG)
    constexpr UINT32 flags = XAUDIO2_DEBUG_ENGINE;
#else
    // NOTE: The debugging facilities are always available for XAudio2 in Windows 8 and newer.
    constexpr UINT32 flags = 0;
#endif

    if (auto hr = ::XAudio2Create(&xAudio2, flags, XAUDIO2_DEFAULT_PROCESSOR); FAILED(hr)) {
        ::CoUninitialize();
        return errors::make("XAudio2Create() failed: " + std::to_string(hr));
    }

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    {
        XAUDIO2_DEBUG_CONFIGURATION debugConfig;
        debugConfig.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
        debugConfig.BreakMask = 0;
        debugConfig.LogThreadID = FALSE;
        debugConfig.LogFileline = FALSE;
        debugConfig.LogFunctionName = FALSE;
        debugConfig.LogTiming = FALSE;

        POMDOG_ASSERT(xAudio2 != nullptr);
        xAudio2->SetDebugConfiguration(&debugConfig, nullptr);
    }
#endif

    auto [audioDevices, enumerateErr] = EnumerateAudioDevices();
    if (enumerateErr != nullptr) {
        xAudio2.Reset();
        ::CoUninitialize();
        return errors::wrap(std::move(enumerateErr), "EnumerateAudioDevices() failed.");
    }

    wchar_t* deviceID = nullptr;

#if 0
    // FIXME
    if (audioDevices.empty()) {
        xAudio2.Reset();
        ::CoUninitialize();
        return errors::make("no audio devices is installed.");
    }
    POMDOG_ASSERT(!audioDevices.empty());
    deviceID = audioDevices.front().DeviceID.data();
#endif

    const auto hr = xAudio2->CreateMasteringVoice(
        &mainVoice,
        XAUDIO2_DEFAULT_CHANNELS,
        XAUDIO2_DEFAULT_SAMPLERATE,
        0,
        deviceID,
        nullptr,
        AudioCategory_GameEffects);

    if (FAILED(hr)) {
        xAudio2.Reset();
        ::CoUninitialize();
        return errors::make("CreateMasteringVoice() failed: " + std::to_string(hr));
    }

    return nullptr;
}

AudioEngineXAudio2::~AudioEngineXAudio2() noexcept
{
    if (mainVoice != nullptr) {
        mainVoice->DestroyVoice();
        mainVoice = nullptr;
    }

    if (xAudio2 != nullptr) {
        xAudio2.Reset();
        ::CoUninitialize();
    }
}

std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
AudioEngineXAudio2::CreateAudioClip(
    const void* audioData,
    std::size_t sizeInBytes,
    int sampleRate,
    int bitsPerSample,
    AudioChannels channels) noexcept
{
    auto audioClip = std::make_shared<AudioClipXAudio2>();

    if (auto err = audioClip->Initialize(audioData, sizeInBytes, sampleRate, bitsPerSample, channels); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize AudioClipXAudio2"));
    }

    return std::make_tuple(std::move(audioClip), nullptr);
}

std::tuple<std::shared_ptr<SoundEffect>, std::unique_ptr<Error>>
AudioEngineXAudio2::CreateSoundEffect(
    const std::shared_ptr<AudioClip>& audioClip,
    bool isLooped) noexcept
{
    POMDOG_ASSERT(audioClip != nullptr);
    POMDOG_ASSERT(std::dynamic_pointer_cast<AudioClipXAudio2>(audioClip) != nullptr);

    auto nativeAudioClip = std::static_pointer_cast<AudioClipXAudio2>(audioClip);
    auto soundEffect = std::make_shared<SoundEffectXAudio2>();

    if (auto err = soundEffect->Initialize(xAudio2.Get(), nativeAudioClip, isLooped); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize SoundEffectXAudio2"));
    }

    return std::make_tuple(std::move(soundEffect), nullptr);
}

float AudioEngineXAudio2::GetMainVolume() const noexcept
{
    float volume = 0.0f;
    if (xAudio2 && mainVoice != nullptr) {
        mainVoice->GetVolume(&volume);
    }
    return volume;
}

void AudioEngineXAudio2::SetMainVolume(float volumeIn) noexcept
{
    if (xAudio2 && mainVoice != nullptr) {
        mainVoice->SetVolume(math::Saturate(volumeIn), XAUDIO2_COMMIT_NOW);
    }
}

IXAudio2* AudioEngineXAudio2::GetXAudio2Engine() const noexcept
{
    POMDOG_ASSERT(xAudio2 != nullptr);
    return xAudio2.Get();
}

} // namespace pomdog::detail::xaudio2
