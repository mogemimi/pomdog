// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/directinput/gamepad_directinput.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/backends/gamepad_helper.h"
#include "pomdog/logging/log.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/exception.h"
#include "pomdog/utility/scope_guard.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <oleauto.h>
#include <wbemidl.h>
#include <algorithm>
#include <array>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::directinput {
namespace {

constexpr LONG ThumbStickMinValue = -32768;
constexpr LONG ThumbStickMaxValue = 32767;

BOOL CALLBACK enumGamepadsCallback(LPCDIDEVICEINSTANCE deviceInstance, LPVOID context)
{
    auto input = reinterpret_cast<GamepadDirectInput*>(context);
    POMDOG_ASSERT(input != nullptr);
    return input->onDeviceAttached(deviceInstance);
}

BOOL CALLBACK enumAxesCallback(const DIDEVICEOBJECTINSTANCE* deviceObject, VOID* context)
{
    auto gamepad = reinterpret_cast<GamepadDevice*>(context);
    POMDOG_ASSERT(gamepad != nullptr);

    if ((deviceObject->dwType & DIDFT_BUTTON) != 0) {
        auto& buttons = gamepad->mappings.buttons;

        constexpr int usageButton0 = 0x01;
        auto buttonIndex = static_cast<int>(deviceObject->wUsage) - usageButton0;

        if ((buttonIndex < 0) || (buttonIndex >= static_cast<int>(buttons.size()))) {
            return DIENUM_CONTINUE;
        }

        POMDOG_ASSERT(buttonIndex >= 0);
        POMDOG_ASSERT(buttonIndex < static_cast<int>(buttons.size()));

        if (auto hasButton = gamepad_mappings::hasButton(gamepad->caps, buttons, buttonIndex); hasButton != nullptr) {
            (*hasButton) = true;
        }
    }
    else if ((deviceObject->dwType & DIDFT_AXIS) != 0) {
        DIPROPRANGE rangeProps;
        rangeProps.diph.dwSize = sizeof(rangeProps);
        rangeProps.diph.dwHeaderSize = sizeof(rangeProps.diph);
        rangeProps.diph.dwHow = DIPH_BYID;
        rangeProps.diph.dwObj = deviceObject->dwType;
        rangeProps.lMin = ThumbStickMinValue;
        rangeProps.lMax = ThumbStickMaxValue;

        auto inputDevice = gamepad->inputDevice;
        POMDOG_ASSERT(inputDevice != nullptr);

        // NOTE: Set the range for the axis
        HRESULT hr = inputDevice->SetProperty(DIPROP_RANGE, &rangeProps.diph);
        if (FAILED(hr)) {
            Log::Warning("Pomdog.Input", "Failed to call IDirectInputDevice8::SetProperty");
            return DIENUM_STOP;
        }

        constexpr int usageAxisX = 0x30;
        const auto axisIndex = static_cast<int>(deviceObject->wUsage) - usageAxisX;

        if ((axisIndex < 0) || (axisIndex >= static_cast<int>(gamepad->mappings.axes.size()))) {
            return DIENUM_CONTINUE;
        }

        POMDOG_ASSERT(axisIndex >= 0);
        POMDOG_ASSERT(axisIndex < static_cast<int>(gamepad->mappings.axes.size()));

        auto& mapper = gamepad->mappings.axes[axisIndex];
        auto& caps = gamepad->caps;

        if (auto hasThumbStick = gamepad_mappings::hasThumbStick(caps, mapper.thumbStick); hasThumbStick != nullptr) {
            (*hasThumbStick) = true;
        }
        if (auto hasButton = gamepad_mappings::hasButton(caps, mapper.positiveTrigger); hasButton != nullptr) {
            (*hasButton) = true;
        }
        if (auto hasButton = gamepad_mappings::hasButton(caps, mapper.negativeTrigger); hasButton != nullptr) {
            (*hasButton) = true;
        }

        POMDOG_ASSERT(axisIndex >= 0);
        POMDOG_ASSERT(axisIndex < static_cast<int>(gamepad->thumbStickInfos.size()));

        auto& info = gamepad->thumbStickInfos[axisIndex];

        switch (mapper.thumbStick) {
        case ThumbStickKind::LeftStickY:
        case ThumbStickKind::RightStickY:
            // Set to -1 to reverse the Y axis (vertical)
            info.InvertDirection = -1;
            break;
        default:
            info.InvertDirection = 1;
            break;
        }
    }

    return DIENUM_CONTINUE;
}

[[nodiscard]] bool
isXInputDevice(const GUID& guidProduct)
{
    auto hr = ::CoInitialize(nullptr);

    const bool cleanupCOM = SUCCEEDED(hr);
    detail::ScopeGuard deferCleanupCOM([&] {
        if (cleanupCOM) {
            ::CoUninitialize();
        }
    });

    Microsoft::WRL::ComPtr<IWbemLocator> wbemLocator = nullptr;
    hr = ::CoCreateInstance(
        __uuidof(WbemLocator),
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(IWbemLocator),
        &wbemLocator);
    if (FAILED(hr) || (wbemLocator == nullptr)) {
        return false;
    }

    const auto bstrNamespace = ::SysAllocString(L"\\\\.\\root\\cimv2");
    if (bstrNamespace == nullptr) {
        return false;
    }
    detail::ScopeGuard defer1([&] { ::SysFreeString(bstrNamespace); });

    const auto bstrClassName = ::SysAllocString(L"Win32_PNPEntity");
    if (bstrClassName == nullptr) {
        return false;
    }
    detail::ScopeGuard defer2([&] { ::SysFreeString(bstrClassName); });

    const auto bstrDeviceID = ::SysAllocString(L"DeviceID");
    if (bstrDeviceID == nullptr) {
        return false;
    }
    detail::ScopeGuard defer3([&] { ::SysFreeString(bstrDeviceID); });

    Microsoft::WRL::ComPtr<IWbemServices> wbemServices = nullptr;
    hr = wbemLocator->ConnectServer(
        bstrNamespace,
        nullptr,
        nullptr,
        0L,
        0L,
        nullptr,
        nullptr,
        &wbemServices);
    if (FAILED(hr) || (wbemServices == nullptr)) {
        return false;
    }

    ::CoSetProxyBlanket(
        wbemServices.Get(),
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        nullptr,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr,
        EOAC_NONE);

    Microsoft::WRL::ComPtr<IEnumWbemClassObject> enumDevices = nullptr;
    hr = wbemServices->CreateInstanceEnum(bstrClassName, 0, nullptr, &enumDevices);
    if (FAILED(hr) || enumDevices == nullptr) {
        return false;
    }

    for (;;) {
        DWORD deviceCount = 0;

        std::array<IWbemClassObject*, 20> devices;
        std::fill(std::begin(devices), std::end(devices), nullptr);
        detail::ScopeGuard defer4([&] {
            for (auto& device : devices) {
                if (device != nullptr) {
                    device->Release();
                    device = nullptr;
                }
            }
        });

        hr = enumDevices->Next(10000, static_cast<ULONG>(devices.size()), devices.data(), &deviceCount);
        if (FAILED(hr)) {
            return false;
        }

        if (deviceCount == 0) {
            break;
        }

        for (DWORD deviceIndex = 0; deviceIndex < deviceCount; ++deviceIndex) {
            VARIANT var;
            hr = devices[deviceIndex]->Get(bstrDeviceID, 0L, &var, nullptr, nullptr);

            if (SUCCEEDED(hr) && (var.vt == VT_BSTR) && (var.bstrVal != nullptr)) {
                if (::wcsstr(var.bstrVal, L"IG_") == nullptr) {
                    continue;
                }

                DWORD vid = 0;
                const auto strVID = ::wcsstr(var.bstrVal, L"VID_");
                if ((strVID != nullptr) && (::swscanf_s(strVID, L"VID_%4lX", &vid) != 1)) {
                    vid = 0;
                }

                DWORD pid = 0;
                const auto strPID = ::wcsstr(var.bstrVal, L"PID_");
                if ((strPID != nullptr) && (::swscanf_s(strPID, L"PID_%4lX", &pid) != 1)) {
                    pid = 0;
                }

                const auto vidPID = static_cast<unsigned long>(MAKELONG(vid, pid));
                if (vidPID == guidProduct.Data1) {
                    return true;
                }
            }
        }
    }

    return false;
}

[[nodiscard]] bool
getCaps(GamepadDevice& gamepad)
{
    POMDOG_ASSERT(gamepad.inputDevice != nullptr);

    const auto inputDevice = gamepad.inputDevice;
    auto& caps = gamepad.caps;

    DIDEVCAPS deviceCaps;
    deviceCaps.dwSize = sizeof(deviceCaps);
    auto hr = inputDevice->GetCapabilities(&deviceCaps);
    if (FAILED(hr)) {
        Log::Warning("Pomdog.Input", "Failed to call IDirectInputDevice8::GetCapabilities");
        return false;
    }

    if (deviceCaps.dwButtons <= 0) {
        // NOTE: If there is no button in the device, it will be treated as a gamepad.
        return false;
    }

    DIDEVICEINSTANCE deviceInfo;
    deviceInfo.dwSize = sizeof(deviceInfo);
    hr = inputDevice->GetDeviceInfo(&deviceInfo);
    if (FAILED(hr)) {
        Log::Warning("Pomdog.Input", "Failed to call IDirectInputDevice8::GetDeviceInfo");
        return false;
    }

    if (std::memcmp(&deviceInfo.guidProduct.Data4[2], "PIDVID", 6) == 0) {
        constexpr std::uint16_t busUSB = 0x03;
        caps.deviceUUID.busType = busUSB;
        caps.deviceUUID.productID = static_cast<std::uint16_t>((deviceInfo.guidProduct.Data1 & 0xffff0000) >> 16);
        caps.deviceUUID.vendorID = static_cast<std::uint16_t>(deviceInfo.guidProduct.Data1 & 0x0000ffff);
        caps.deviceUUID.versionNumber = 0;
    }
    else {
        // FIXME: Not implemented here
        caps.deviceUUID.busType = 0x05;
        caps.deviceUUID.productID = 0;
        caps.deviceUUID.vendorID = 0;
        caps.deviceUUID.versionNumber = 0;
        Log::Warning(
            "Pomdog.Input",
            "Pomdog does not support bluetooth gamepads yet,"
            "please report a issue to GitHub: https://github.com/mogemimi/pomdog .");
    }

    auto uuidString = caps.deviceUUID.toString();
    if (gamepad.isXInputDevice) {
        // NOTE: Rename uuid string for SDL GameController DB
        // https://github.com/gabomdq/SDL_GameControllerDB/blob/6854c08b4643a857e23e1d22d55a41d86ff63150/data/SDL_gamecontrollerdb2.0.6.h#L35
        uuidString = "xinput";
    }

    std::tie(gamepad.mappings, caps.name) = gamepad_mappings::getMappings(uuidString);
    if (caps.name.empty() || gamepad.isXInputDevice) {
        caps.name = deviceInfo.tszProductName;
    }

    if (gamepad.isXInputDevice) {
        // NOTE: According to the following page, on an XInput device,
        // the left and right trigger buttons will act as a single button, not independently.
        // https://docs.microsoft.com/en-us/windows/desktop/xinput/xinput-and-directinput
        // FIXME: Use XInput instead of DirectInput
        for (auto& axis : gamepad.mappings.axes) {
            if (axis.positiveTrigger == ButtonKind::RightTrigger) {
                axis.positiveTrigger = ButtonKind::Invalid;
            }
            if ((axis.positiveTrigger == ButtonKind::LeftTrigger) &&
                (axis.negativeTrigger == ButtonKind::Invalid)) {
                axis.negativeTrigger = ButtonKind::RightTrigger;
            }
        }
    }

    hr = inputDevice->EnumObjects(
        enumAxesCallback,
        static_cast<LPVOID>(&gamepad),
        DIDFT_BUTTON | DIDFT_AXIS);

    if (FAILED(hr)) {
        Log::Warning("Pomdog.Input", "Failed to call IDirectInputDevice8::EnumObjects");
        return false;
    }

    DIPROPDWORD props;
    props.diph.dwSize = sizeof(props);
    props.diph.dwHeaderSize = sizeof(props.diph);
    props.diph.dwHow = DIPH_DEVICE;
    props.diph.dwObj = 0;
    props.dwData = DIPROPAXISMODE_ABS;
    hr = inputDevice->SetProperty(DIPROP_AXISMODE, &props.diph);
    if (FAILED(hr)) {
        Log::Warning("Pomdog.Input", "Failed to call IDirectInputDevice8::SetProperty");
        return false;
    }

    return true;
}

void onNotAcquired(GamepadDevice& gamepad)
{
    POMDOG_ASSERT(gamepad.inputDevice != nullptr);
    HRESULT hr = gamepad.inputDevice->Acquire();
    if (FAILED(hr)) {
        if (DIERR_OTHERAPPHASPRIO != hr) {
            Log::Warning("Pomdog.Input", "Failed to call IDirectInputDevice8::Acquire");
            gamepad.state.isConnected = false;
            return;
        }
        gamepad.deviceState = GamepadStateDirectInput::NotAcquired;
        return;
    }
    gamepad.deviceState = GamepadStateDirectInput::Acquired;
}

[[nodiscard]] float
normalizeAxisValue(LONG value, const ThumbStickInfo& info)
{
    static_assert(ThumbStickMaxValue >= ThumbStickMinValue, "");
    constexpr auto range = ThumbStickMaxValue - ThumbStickMinValue;
    return static_cast<float>(info.InvertDirection * ((value - ThumbStickMinValue) * 2 - range)) / range;
}

} // namespace

bool GamepadDevice::open(IDirectInput8* directInput, HWND windowHandle, const ::GUID& guidInstance)
{
    POMDOG_ASSERT(inputDevice == nullptr);
    POMDOG_ASSERT(directInput != nullptr);

    auto hr = directInput->CreateDevice(guidInstance, &inputDevice, nullptr);
    if (FAILED(hr)) {
        Log::Warning("Pomdog.Input", "Failed to call IDirectInput8::CreateDevice");
        return false;
    }

    POMDOG_ASSERT(inputDevice != nullptr);

    // TODO: Replace c_dfDIJoystick with c_dfDIJoystick2
    hr = inputDevice->SetDataFormat(&c_dfDIJoystick);
    if (FAILED(hr)) {
        Log::Warning("Pomdog.Input", "Failed to call IDirectInputDevice8::SetDataFormat");
        close();
        return false;
    }

    hr = inputDevice->SetCooperativeLevel(windowHandle, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
    if (FAILED(hr)) {
        Log::Warning("Pomdog.Input", "Failed to call IDirectInputDevice8::SetCooperativeLevel");
        close();
        return false;
    }

    if (!getCaps(*this)) {
        Log::Warning("Pomdog.Input", "Failed to get capabilities");
        close();
        return false;
    }

    deviceState = GamepadStateDirectInput::NotAcquired;

    hr = inputDevice->Acquire();
    if (FAILED(hr)) {
        if (DIERR_OTHERAPPHASPRIO != hr) {
            Log::Warning("Pomdog.Input", "Failed to call IDirectInputDevice8::Acquire");
            close();
            return false;
        }
        POMDOG_ASSERT(deviceState == GamepadStateDirectInput::NotAcquired);
    }
    else {
        deviceState = GamepadStateDirectInput::Acquired;
    }

    GamepadHelper::clearState(state);
    state.isConnected = true;

    Log::Internal("Open gamepad: " + caps.name);

    return true;
}

void GamepadDevice::close()
{
    if (inputDevice != nullptr) {
        inputDevice->Unacquire();
        inputDevice.Reset();
    }

    GamepadHelper::clearState(state);
    state.isConnected = false;

    GamepadCapabilities emptyCaps;
    std::swap(caps, emptyCaps);

    deviceState = GamepadStateDirectInput::NotInitialized;
    deviceGuid.Data1 = 0;
    deviceGuid.Data2 = 0;
    deviceGuid.Data3 = 0;
    std::fill(std::begin(deviceGuid.Data4), std::end(deviceGuid.Data4), static_cast<unsigned char>(0));
    isXInputDevice = false;
}

void GamepadDevice::pollEvents()
{
    if (deviceState == GamepadStateDirectInput::NotInitialized) {
        return;
    }

    POMDOG_ASSERT(inputDevice != nullptr);

    switch (deviceState) {
    case GamepadStateDirectInput::NotAcquired:
        onNotAcquired(*this);
        break;
    case GamepadStateDirectInput::Acquired:
        break;
    case GamepadStateDirectInput::NotInitialized:
        break;
    }

    if (deviceState != GamepadStateDirectInput::Acquired) {
        return;
    }

    auto hr = inputDevice->Poll();
    if (FAILED(hr)) {
        deviceState = GamepadStateDirectInput::NotAcquired;
        return;
    }

    // TODO: Replace DIJOYSTATE with DIJOYSTATE2
    DIJOYSTATE joystate;
    hr = inputDevice->GetDeviceState(sizeof(DIJOYSTATE), &joystate);

    if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)) {
        hr = inputDevice->Acquire();
        if (FAILED(hr)) {
            deviceState = GamepadStateDirectInput::NotAcquired;
            return;
        }
        hr = inputDevice->GetDeviceState(sizeof(DIJOYSTATE), &joystate);
    }

    switch (hr) {
    case DI_OK:
        break;
    case DIERR_NOTACQUIRED:
        deviceState = GamepadStateDirectInput::NotAcquired;
        return;
    default:
        state.isConnected = false;
        return;
    }

    for (int i = 0; i < static_cast<int>(mappings.buttons.size()); ++i) {
        if (auto button = gamepad_mappings::getButton(state, mappings.buttons, i); button != nullptr) {
            (*button) = ((joystate.rgbButtons[i] & 0x80) != 0)
                            ? ButtonState::Pressed
                            : ButtonState::Released;
        }
    }

    const auto pov = joystate.rgdwPOV[0] / 100;
    state.dpad.up = ButtonState::Released;
    state.dpad.down = ButtonState::Released;
    state.dpad.left = ButtonState::Released;
    state.dpad.right = ButtonState::Released;

    static_assert(std::is_unsigned_v<decltype(pov)>, "0 <= pov");

    if ((pov <= 45) || ((315 <= pov) && (pov <= 360))) {
        state.dpad.up = ButtonState::Pressed;
    }
    else if ((135 <= pov) && (pov <= 225)) {
        state.dpad.down = ButtonState::Pressed;
    }

    if ((45 <= pov) && (pov <= 135)) {
        state.dpad.right = ButtonState::Pressed;
    }
    else if ((225 <= pov) && (pov <= 315)) {
        state.dpad.left = ButtonState::Pressed;
    }

    const std::array<LONG, 6> values = {{
        joystate.lX,
        joystate.lY,
        joystate.lZ,
        joystate.lRx,
        joystate.lRy,
        joystate.lRz,
    }};

    for (int i = 0; i < static_cast<int>(mappings.axes.size()); ++i) {
        POMDOG_ASSERT(i < static_cast<int>(values.size()));
        POMDOG_ASSERT(i < static_cast<int>(mappings.axes.size()));
        POMDOG_ASSERT(i < static_cast<int>(thumbStickInfos.size()));

        const auto& mapper = mappings.axes[i];

        if (auto thumbStick = gamepad_mappings::getThumbStick(state, mapper.thumbStick); thumbStick != nullptr) {
            (*thumbStick) = normalizeAxisValue(values[i], thumbStickInfos[i]);
        }

        if (auto button = gamepad_mappings::getButton(state, mapper.positiveTrigger); button != nullptr) {
            const auto value = normalizeAxisValue(values[i], thumbStickInfos[i]);
            constexpr float threshold = 0.05f;
            (*button) = (value > threshold) ? ButtonState::Pressed : ButtonState::Released;
        }

        if (auto button = gamepad_mappings::getButton(state, mapper.negativeTrigger); button != nullptr) {
            const auto value = normalizeAxisValue(values[i], thumbStickInfos[i]);
            constexpr float threshold = -0.05f;
            (*button) = (value < threshold) ? ButtonState::Pressed : ButtonState::Released;
        }
    }
}

std::unique_ptr<Error>
GamepadDirectInput::initialize(HINSTANCE hInstance, HWND windowHandleIn) noexcept
{
    windowHandle_ = windowHandleIn;
    directInput_ = nullptr;

    POMDOG_ASSERT(hInstance != nullptr);
    POMDOG_ASSERT(windowHandle_ != nullptr);

    gamepads_[0].playerIndex = PlayerIndex::One;
    gamepads_[1].playerIndex = PlayerIndex::Two;
    gamepads_[2].playerIndex = PlayerIndex::Three;
    gamepads_[3].playerIndex = PlayerIndex::Four;

    if (auto hr = ::DirectInput8Create(
            hInstance,
            DIRECTINPUT_VERSION,
            IID_IDirectInput8,
            &directInput_,
            nullptr);
        FAILED(hr)) {
        return errors::make("DirectInput8Create() failed");
    }

    for (auto& gamepad : gamepads_) {
        gamepad.deviceState = GamepadStateDirectInput::NotInitialized;
    }

    return nullptr;
}

GamepadDirectInput::~GamepadDirectInput()
{
    for (auto& gamepad : gamepads_) {
        gamepad.close();
    }
    directInput_.Reset();
    windowHandle_ = nullptr;
}

void GamepadDirectInput::enumerateDevices()
{
    POMDOG_ASSERT(directInput_ != nullptr);
    POMDOG_ASSERT(windowHandle_ != nullptr);

    std::lock_guard<std::mutex> lock(mutex_);

    auto hr = directInput_->EnumDevices(
        DI8DEVCLASS_GAMECTRL,
        enumGamepadsCallback,
        static_cast<LPVOID>(this),
        DIEDFL_ATTACHEDONLY);

    if (FAILED(hr)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Error: Failed to enumerate any input devices.");
    }
}

BOOL GamepadDirectInput::onDeviceAttached(LPCDIDEVICEINSTANCE deviceInstance)
{
    for (auto& gamepad : gamepads_) {
        auto iter = std::find_if(std::begin(gamepads_), std::end(gamepads_), [&](const GamepadDevice& dev) {
            return std::memcmp(&dev.deviceGuid, &deviceInstance->guidInstance, sizeof(dev.deviceGuid)) == 0;
        });
        if (iter != std::end(gamepads_)) {
            // The device is already opened.
            break;
        }

        if (gamepad.deviceState == GamepadStateDirectInput::NotInitialized) {
            gamepad.isXInputDevice = false;
            if (isXInputDevice(deviceInstance->guidProduct)) {
                // TODO: Use XInput instead of DirectInput for Xbox controllers.
                gamepad.isXInputDevice = true;
            }
            if (!gamepad.open(directInput_.Get(), windowHandle_, deviceInstance->guidInstance)) {
                Log::Warning("Pomdog.Input", "Failed to initialize gamepad");
                return DIENUM_CONTINUE;
            }
            std::memcpy(&gamepad.deviceGuid, &deviceInstance->guidInstance, sizeof(gamepad.deviceGuid));
            this->Connected(gamepad.playerIndex, gamepad.caps);
            return DIENUM_CONTINUE;
        }
    }
    return DIENUM_STOP;
}

GamepadCapabilities
GamepadDirectInput::getCapabilities(PlayerIndex playerIndex) const
{
    const auto index = GamepadHelper::toInt(playerIndex);
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(gamepads_.size()));
    return gamepads_[index].caps;
}

GamepadState
GamepadDirectInput::getState(PlayerIndex playerIndex) const
{
    const auto index = GamepadHelper::toInt(playerIndex);
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(gamepads_.size()));
    return gamepads_[index].state;
}

void GamepadDirectInput::pollEvents()
{
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto& gamepad : gamepads_) {
        if (!gamepad.state.isConnected) {
            continue;
        }

        gamepad.pollEvents();
        if (!gamepad.state.isConnected) {
            auto caps = gamepad.caps;
            gamepad.close();
            this->Disconnected(gamepad.playerIndex, caps);
        }
    }
}

} // namespace pomdog::detail::directinput
