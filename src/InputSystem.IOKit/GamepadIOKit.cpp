// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GamepadIOKit.hpp"
#include "../Application/SystemEvents.hpp"
#include "../InputSystem/GamepadHelper.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Signals/Event.hpp"
#include "Pomdog/Signals/EventQueue.hpp"
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

float NormalizeAxisValue(IOHIDValueRef valueRef, const ThumbStickInfo& info)
{
    const auto value = IOHIDValueGetIntegerValue(valueRef);
    return static_cast<float>(info.InvertDirection * ((value - info.Minimum) * 2 - info.Range)) / info.Range;
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

    GamepadCapabilities emptyCaps;
    std::swap(caps, emptyCaps);
}

void GamepadDevice::OnDeviceInput(IOReturn result, void* sender, IOHIDValueRef valueRef)
{
    const auto element = IOHIDValueGetElement(valueRef);
    const auto usagePage = IOHIDElementGetUsagePage(element);
    const auto usage = IOHIDElementGetUsage(element);

    switch (IOHIDElementGetType(element)) {
    case kIOHIDElementTypeInput_Button: {
        const auto buttonIndex = static_cast<int>(usage) - 1;
        if (auto button = GetButton(state, mappings.buttons, buttonIndex); button != nullptr) {
            (*button) = (IOHIDValueGetIntegerValue(valueRef) != 0)
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
                const auto axisIndex = static_cast<int>(usage - kHIDUsage_GD_X);
                POMDOG_ASSERT(axisIndex >= 0);
                POMDOG_ASSERT(axisIndex < static_cast<int>(mappings.axes.size()));
                POMDOG_ASSERT(axisIndex < static_cast<int>(thumbStickInfos.size()));

                const auto& mapper = mappings.axes[axisIndex];

                if (auto thumbStick = GetThumbStick(state, mapper.thumbStick); thumbStick != nullptr) {
                    (*thumbStick) = NormalizeAxisValue(valueRef, thumbStickInfos[axisIndex]);
                }

                if (auto button = GetButton(state, mapper.positiveTrigger); button != nullptr) {
                    const auto value = NormalizeAxisValue(valueRef, thumbStickInfos[axisIndex]);
                    constexpr float threshold = 0.05f;
                    (*button) = (value > threshold) ? ButtonState::Pressed : ButtonState::Released;
                }

                if (auto button = GetButton(state, mapper.negativeTrigger); button != nullptr) {
                    const auto value = NormalizeAxisValue(valueRef, thumbStickInfos[axisIndex]);
                    constexpr float threshold = -0.05f;
                    (*button) = (value < threshold) ? ButtonState::Pressed : ButtonState::Released;
                }

                break;
            }
            case kHIDUsage_GD_Hatswitch: {
                state.DPad.Up = ButtonState::Released;
                state.DPad.Down = ButtonState::Released;
                state.DPad.Left = ButtonState::Released;
                state.DPad.Right = ButtonState::Released;

                switch (IOHIDValueGetIntegerValue(valueRef)) {
                case 0:
                    state.DPad.Up = ButtonState::Pressed;
                    break;
                case 1:
                    state.DPad.Up = ButtonState::Pressed;
                    state.DPad.Right = ButtonState::Pressed;
                    break;
                case 2:
                    state.DPad.Right = ButtonState::Pressed;
                    break;
                case 3:
                    state.DPad.Right = ButtonState::Pressed;
                    state.DPad.Down = ButtonState::Pressed;
                    break;
                case 4:
                    state.DPad.Down = ButtonState::Pressed;
                    break;
                case 5:
                    state.DPad.Down = ButtonState::Pressed;
                    state.DPad.Left = ButtonState::Pressed;
                    break;
                case 6:
                    state.DPad.Left = ButtonState::Pressed;
                    break;
                case 7:
                    state.DPad.Up = ButtonState::Pressed;
                    state.DPad.Left = ButtonState::Pressed;
                    break;
                default:
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

GamepadIOKit::GamepadIOKit(const std::shared_ptr<EventQueue>& eventQueueIn)
    : eventQueue(eventQueueIn)
    , hidManager(nullptr)
{
    gamepads[0].playerIndex = PlayerIndex::One;
    gamepads[1].playerIndex = PlayerIndex::Two;
    gamepads[2].playerIndex = PlayerIndex::Three;
    gamepads[3].playerIndex = PlayerIndex::Four;

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
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(gamepads.size()));
    return gamepads[index].caps;
}

GamepadState GamepadIOKit::GetState(PlayerIndex playerIndex) const
{
    const auto index = GamepadHelper::ToInt(playerIndex);
    POMDOG_ASSERT(index >= 0);
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

    std::int32_t vendor = 0;
    std::int32_t product = 0;
    std::int32_t version = 0;

    auto vendorIDKey = reinterpret_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVendorIDKey)));
    if (vendorIDKey != nullptr) {
        CFNumberGetValue(vendorIDKey, kCFNumberSInt32Type, &vendor);
    }

    auto productIDKey = reinterpret_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductIDKey)));
    if (productIDKey != nullptr) {
        CFNumberGetValue(productIDKey, kCFNumberSInt32Type, &product);
    }

    auto versionNumberKey = reinterpret_cast<CFNumberRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVersionNumberKey)));
    if (versionNumberKey != nullptr) {
        CFNumberGetValue(versionNumberKey, kCFNumberSInt32Type, &version);
    }

    constexpr std::uint16_t busUSB = 0x03;

    auto& uuid = gamepad->caps.DeviceUUID;
    uuid.BusType = busUSB;
    uuid.VendorID = static_cast<uint16_t>(vendor);
    uuid.ProductID = static_cast<uint16_t>(product);
    uuid.VersionNumber = static_cast<uint16_t>(version);

    std::tie(gamepad->mappings, gamepad->caps.Name) = GetMappings(uuid);
    if (gamepad->caps.Name.empty()) {
        auto productKey = reinterpret_cast<CFStringRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductKey)));
        if (productKey != nullptr) {
            std::array<char, 256> buffer;
            std::fill(std::begin(buffer), std::end(buffer), 0);
            CFStringGetCString(productKey, buffer.data(), buffer.size(), kCFStringEncodingUTF8);
            gamepad->caps.Name = buffer.data();
        }
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
            case kIOHIDElementTypeInput_Button: {
                const auto buttonIndex = static_cast<int>(usage) - 1;
                if (auto hasButton = HasButton(gamepad->caps, gamepad->mappings.buttons, buttonIndex); hasButton != nullptr) {
                    (*hasButton) = true;
                }
                break;
            }
            case kIOHIDElementTypeInput_Axis:
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
                        const auto axisIndex = static_cast<int>(usage - kHIDUsage_GD_X);

                        POMDOG_ASSERT(axisIndex >= 0);
                        POMDOG_ASSERT(axisIndex < static_cast<int>(gamepad->mappings.axes.size()));

                        const auto& mapper = gamepad->mappings.axes[axisIndex];

                        if (auto hasThumbStick = HasThumbStick(gamepad->caps, mapper.thumbStick); hasThumbStick != nullptr) {
                            (*hasThumbStick) = true;
                        }
                        if (auto hasButton = HasButton(gamepad->caps, mapper.positiveTrigger); hasButton != nullptr) {
                            (*hasButton) = true;
                        }
                        if (auto hasButton = HasButton(gamepad->caps, mapper.negativeTrigger); hasButton != nullptr) {
                            (*hasButton) = true;
                        }

                        POMDOG_ASSERT(axisIndex >= 0);
                        POMDOG_ASSERT(axisIndex < static_cast<int>(gamepad->thumbStickInfos.size()));

                        const auto minimum = static_cast<int32_t>(IOHIDElementGetLogicalMin(element));
                        const auto maximum = static_cast<int32_t>(IOHIDElementGetLogicalMax(element));
                        auto& info = gamepad->thumbStickInfos[axisIndex];
                        info.Minimum = minimum;
                        info.Range = std::max(1, maximum - minimum);
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
    eventQueue->Enqueue<GamepadConnectedEvent>(gamepad->playerIndex, gamepad->caps);

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

    auto caps = gamepad->caps;
    gamepad->Close();
    eventQueue->Enqueue<GamepadDisconnectedEvent>(gamepad->playerIndex, caps);
}

void GamepadIOKit::HandleEvent(const Event& event)
{
    if (auto connected = event.As<GamepadConnectedEvent>()) {
        Connected(connected->PlayerIndex, connected->Capabilities);
    }
    else if (auto disconnected = event.As<GamepadDisconnectedEvent>()) {
        Disconnected(disconnected->PlayerIndex, disconnected->Capabilities);
    }
}

} // namespace Apple
} // namespace InputSystem
} // namespace Detail
} // namespace Pomdog
