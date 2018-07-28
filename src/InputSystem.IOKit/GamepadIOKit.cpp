// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GamepadIOKit.hpp"
#include "../InputSystem/GamepadHelper.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <algorithm>

namespace Pomdog {
namespace Detail {
namespace InputSystem {
namespace Apple {
namespace {

void AppendDeviceMatching(CFMutableArrayRef matcher, uint32_t page, uint32_t usage)
{
    auto result = CFDictionaryCreateMutable(
        kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

    if (result == nullptr) {
        return;
    }

    auto pageNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &page);
    CFDictionarySetValue(result, CFSTR(kIOHIDDeviceUsagePageKey), pageNumber);
    CFRelease(pageNumber);

    auto usageNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage);
    CFDictionarySetValue(result, CFSTR(kIOHIDDeviceUsageKey), usageNumber);
    CFRelease(usageNumber);

    CFArrayAppendValue(matcher, result);
    CFRelease(result);
}

ButtonState GetTriggerButtonValue(IOHIDValueRef valueRef)
{
    const auto value = IOHIDValueGetIntegerValue(valueRef);
    return (value > 0) ? ButtonState::Pressed: ButtonState::Released;
}

float GetThumbStickValue(IOHIDValueRef valueRef, const ThumbStickInfo& info)
{
    const auto value = IOHIDValueGetIntegerValue(valueRef);
    return (static_cast<float>((value - info.Minimum) * 2 - info.Range) / info.Range);
}

} // unnamed namespace

void GamepadDevice::Close()
{
    if (device != nullptr) {
        IOHIDDeviceClose(device, kIOHIDOptionsTypeNone);
        device = nullptr;
    }

    GamepadHelper::ClearState(state);
    state.IsConnected = false;

    caps.Name = "";
    caps.ThumbStickCount = 0;
    caps.ButtonCount = 0;
}

void GamepadDevice::OnDeviceInput(IOReturn result, void* sender, IOHIDValueRef value)
{
    const auto element = IOHIDValueGetElement(value);
    const auto usagePage = IOHIDElementGetUsagePage(element);
    const auto usage = IOHIDElementGetUsage(element);

    switch (IOHIDElementGetType(element)) {
    case kIOHIDElementTypeInput_Button: {
        const auto buttonIndex = static_cast<int>(usage) - 1;
        if (auto button = GetButton(state, mappings.buttons, buttonIndex); button != nullptr) {
            (*button) = (IOHIDValueGetIntegerValue(value) != 0)
                ? ButtonState::Pressed
                : ButtonState::Released;
        }
        break;
    }
    case kIOHIDElementTypeInput_Axis:
    case kIOHIDElementTypeInput_Misc: {
        switch (usagePage) {
        case kHIDPage_GenericDesktop: {
            switch (usage) {
            case kHIDUsage_GD_X:
            case kHIDUsage_GD_Y:
            case kHIDUsage_GD_Z:
            case kHIDUsage_GD_Rx:
            case kHIDUsage_GD_Ry:
            case kHIDUsage_GD_Rz: {
                const auto thumbStickIndex = static_cast<int>(usage - kHIDUsage_GD_X);
                POMDOG_ASSERT(thumbStickIndex >= 0);
                POMDOG_ASSERT(thumbStickIndex < static_cast<int>(mappings.axes.size()));
                POMDOG_ASSERT(thumbStickIndex < static_cast<int>(thumbStickInfos.size()));

                switch (mappings.axes[thumbStickIndex]) {
                case AxesKind::LeftTrigger:
                    state.Buttons.LeftTrigger = GetTriggerButtonValue(value);
                    break;
                case AxesKind::RightTrigger:
                    state.Buttons.RightTrigger = GetTriggerButtonValue(value);
                    break;
                case AxesKind::LeftStickX:
                    state.ThumbSticks.Left.X = GetThumbStickValue(value, thumbStickInfos[thumbStickIndex]);
                    break;
                case AxesKind::LeftStickY:
                    state.ThumbSticks.Left.Y = GetThumbStickValue(value, thumbStickInfos[thumbStickIndex]);
                    break;
                case AxesKind::RightStickX:
                    state.ThumbSticks.Right.X = GetThumbStickValue(value, thumbStickInfos[thumbStickIndex]);
                    break;
                case AxesKind::RightStickY:
                    state.ThumbSticks.Right.Y = GetThumbStickValue(value, thumbStickInfos[thumbStickIndex]);
                    break;
                default:
                    break;
                }
                break;
            }
            case kHIDUsage_GD_Hatswitch: {
                // TODO: Need to refactor later.
                switch (IOHIDValueGetIntegerValue(value)) {
                case 0:
                    state.DPad.Up = ButtonState::Pressed;
                    state.DPad.Right = ButtonState::Released;
                    state.DPad.Down = ButtonState::Released;
                    state.DPad.Left = ButtonState::Released;
                    break;
                case 1:
                    state.DPad.Up = ButtonState::Pressed;
                    state.DPad.Right = ButtonState::Pressed;
                    state.DPad.Down = ButtonState::Released;
                    state.DPad.Left = ButtonState::Released;
                    break;
                case 2:
                    state.DPad.Up = ButtonState::Released;
                    state.DPad.Right = ButtonState::Pressed;
                    state.DPad.Down = ButtonState::Released;
                    state.DPad.Left = ButtonState::Released;
                    break;
                case 3:
                    state.DPad.Up = ButtonState::Released;
                    state.DPad.Right = ButtonState::Pressed;
                    state.DPad.Down = ButtonState::Pressed;
                    state.DPad.Left = ButtonState::Released;
                    break;
                case 4:
                    state.DPad.Up = ButtonState::Released;
                    state.DPad.Right = ButtonState::Released;
                    state.DPad.Down = ButtonState::Pressed;
                    state.DPad.Left = ButtonState::Released;
                    break;
                case 5:
                    state.DPad.Up = ButtonState::Released;
                    state.DPad.Right = ButtonState::Released;
                    state.DPad.Down = ButtonState::Pressed;
                    state.DPad.Left = ButtonState::Pressed;
                    break;
                case 6:
                    state.DPad.Up = ButtonState::Released;
                    state.DPad.Right = ButtonState::Released;
                    state.DPad.Down = ButtonState::Released;
                    state.DPad.Left = ButtonState::Pressed;
                    break;
                case 7:
                    state.DPad.Up = ButtonState::Pressed;
                    state.DPad.Right = ButtonState::Released;
                    state.DPad.Down = ButtonState::Released;
                    state.DPad.Left = ButtonState::Pressed;
                    break;
                case 8:
                default:
                    state.DPad.Up = ButtonState::Released;
                    state.DPad.Down = ButtonState::Released;
                    state.DPad.Left = ButtonState::Released;
                    state.DPad.Right = ButtonState::Released;
                    break;
                }
                break;
            }
            case kHIDUsage_GD_DPadUp:
            case kHIDUsage_GD_DPadDown:
            case kHIDUsage_GD_DPadRight:
            case kHIDUsage_GD_DPadLeft:
            case kHIDUsage_GD_Start:
            case kHIDUsage_GD_Select:
            case kHIDUsage_GD_SystemMainMenu: {
                // TODO: Not implemented
                break;
            }
            default:
                break;
            }

            break;
        }
        case kHIDPage_Button:
        case kHIDPage_Consumer: {
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

GamepadIOKit::GamepadIOKit()
    : hidManager(nullptr)
{
    hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (hidManager == nullptr) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Error: Failed to call IOHIDManagerCreate");
    }

    auto deviceMatcher = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
    if (deviceMatcher == nullptr) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Error: Failed to call CFArrayCreateMutable");
    }

    AppendDeviceMatching(deviceMatcher, kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick);
    AppendDeviceMatching(deviceMatcher, kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad);

    IOHIDManagerSetDeviceMatchingMultiple(hidManager, deviceMatcher);
    CFRelease(deviceMatcher);

    IOHIDManagerRegisterDeviceMatchingCallback(
        hidManager,
        [](void* context, IOReturn result, void* sender, IOHIDDeviceRef device) {
            auto c = reinterpret_cast<GamepadIOKit*>(context);
            POMDOG_ASSERT(c != nullptr);
            c->OnDeviceAttached(result, sender, device);
        },
        this);
    IOHIDManagerRegisterDeviceRemovalCallback(
        hidManager,
        [](void* context, IOReturn result, void* sender, IOHIDDeviceRef device) {
            auto c = reinterpret_cast<GamepadIOKit*>(context);
            POMDOG_ASSERT(c != nullptr);
            c->OnDeviceDetached(result, sender, device);
        },
        this);

    IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetMain(), kCFRunLoopCommonModes);
    IOHIDManagerOpen(hidManager, kIOHIDOptionsTypeNone);
}

GamepadIOKit::~GamepadIOKit()
{
    for (auto& device : gamepads) {
        device.Close();
    }

    if (hidManager != nullptr) {
        IOHIDManagerUnscheduleFromRunLoop(hidManager, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
        IOHIDManagerClose(hidManager, kIOHIDOptionsTypeNone);
        CFRelease(hidManager);
        hidManager = nullptr;
    }
}

GamepadCapabilities GamepadIOKit::GetCapabilities(PlayerIndex playerIndex) const
{
    const auto index = GamepadHelper::ToInt(playerIndex);
    POMDOG_ASSERT(index < static_cast<int>(gamepads.size()));
    return gamepads[index].caps;
}

GamepadState GamepadIOKit::GetState(PlayerIndex playerIndex) const
{
    const auto index = GamepadHelper::ToInt(playerIndex);
    POMDOG_ASSERT(index < static_cast<int>(gamepads.size()));
    return gamepads[index].state;
}

void GamepadIOKit::OnDeviceAttached(IOReturn result, void* sender, IOHIDDeviceRef device)
{
    auto gamepad = std::find_if(std::begin(gamepads), std::end(gamepads), [](GamepadDevice& a) {
        return a.device == nullptr;
    });
    if (gamepad == std::end(gamepads)) {
        return;
    }

    GamepadHelper::ClearState(gamepad->state);
    gamepad->device = device;
    gamepad->state.IsConnected = true;

    auto deviceName = reinterpret_cast<CFStringRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductKey)));
    if (deviceName != nullptr) {
        std::array<char, 256> buffer;
        std::fill(std::begin(buffer), std::end(buffer), 0);
        CFStringGetCString(deviceName, buffer.data(), buffer.size(), kCFStringEncodingUTF8);
        gamepad->caps.Name = buffer.data();
    }
    else {
        gamepad->caps.Name = "unknown";
    }

    int32_t vendor = 0;
    int32_t product = 0;
    int32_t version = 0;

    auto vendorID = reinterpret_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVendorIDKey)));
    if (vendorID != nullptr) {
        CFNumberGetValue(vendorID, kCFNumberSInt32Type, &vendor);
    }

    auto productID = reinterpret_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductIDKey)));
    if (productID != nullptr) {
        CFNumberGetValue(productID, kCFNumberSInt32Type, &product);
    }

    auto versionNumber = reinterpret_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVersionNumberKey)));
    if (versionNumber != nullptr) {
        CFNumberGetValue(versionNumber, kCFNumberSInt32Type, &version);
    }

    constexpr uint16_t busUSB = 0x03;

    GamepadDeviceID uuid;
    uuid.BusType = busUSB;
    uuid.Vendor = static_cast<uint16_t>(vendor);
    uuid.Product = static_cast<uint16_t>(product);
    uuid.Version = static_cast<uint16_t>(version);

    std::string controllerName;
    std::tie(gamepad->mappings, controllerName) = GetMappings(uuid);
    if (!controllerName.empty()) {
        gamepad->caps.Name = controllerName;
    }

    auto elements = IOHIDDeviceCopyMatchingElements(device, nullptr, kIOHIDOptionsTypeNone);
    if (elements != nullptr) {
        for (int i = 0; i < CFArrayGetCount(elements); ++i) {
            auto element = (IOHIDElementRef)(CFArrayGetValueAtIndex(elements, i));
            if (CFGetTypeID(element) != IOHIDElementGetTypeID()) {
                continue;
            }

            const auto usagePage = IOHIDElementGetUsagePage(element);
            const auto usage = IOHIDElementGetUsage(element);

            switch (IOHIDElementGetType(element)) {
            case kIOHIDElementTypeInput_Axis:
            case kIOHIDElementTypeInput_Button:
            case kIOHIDElementTypeInput_Misc:
                switch (usagePage) {
                case kHIDPage_GenericDesktop:
                    switch (usage) {
                    case kHIDUsage_GD_X:
                    case kHIDUsage_GD_Y:
                    case kHIDUsage_GD_Z:
                    case kHIDUsage_GD_Rx:
                    case kHIDUsage_GD_Ry:
                    case kHIDUsage_GD_Rz: {
                        const auto thumbStickIndex = static_cast<int>(usage - kHIDUsage_GD_X);
                        POMDOG_ASSERT(thumbStickIndex >= 0);
                        POMDOG_ASSERT(thumbStickIndex < static_cast<int>(gamepad->thumbStickInfos.size()));

                        const auto minimum = static_cast<int32_t>(IOHIDElementGetLogicalMin(element));
                        const auto maximum = static_cast<int32_t>(IOHIDElementGetLogicalMax(element));
                        gamepad->thumbStickInfos[thumbStickIndex].Minimum = minimum;
                        gamepad->thumbStickInfos[thumbStickIndex].Range = std::max(1, maximum - minimum);
                        break;
                    }
                    default:
                        break;
                    }
                    break;
                default:
                    break;
                }
                break;
            case kIOHIDElementTypeCollection:
                break;
            default:
                break;
            }
        }

        CFRelease(elements);
    }

    Log::Internal("Attached device: " + gamepad->caps.Name);

    IOHIDDeviceOpen(device, kIOHIDOptionsTypeNone);
    IOHIDDeviceScheduleWithRunLoop(device, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
    IOHIDDeviceRegisterInputValueCallback(
        device,
        [](void* context, IOReturn result, void* sender, IOHIDValueRef value) {
            auto c = reinterpret_cast<GamepadDevice*>(context);
            POMDOG_ASSERT(c != nullptr);
            c->OnDeviceInput(result, sender, value);
        },
        &(*gamepad));
}

void GamepadIOKit::OnDeviceDetached(IOReturn result, void* sender, IOHIDDeviceRef device)
{
    auto gamepad = std::find_if(std::begin(gamepads), std::end(gamepads), [&](GamepadDevice& a) {
        return a.device == device;
    });
    if (gamepad == std::end(gamepads)) {
        Log::Internal("The device was not found");
        return;
    }

    Log::Internal("Detached device: " + gamepad->caps.Name);
    gamepad->Close();
}

} // namespace Apple
} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog
