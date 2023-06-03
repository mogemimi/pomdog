// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/iokit/gamepad_iokit.h"
#include "pomdog/input/backends/gamepad_helper.h"
#include "pomdog/logging/log.h"
#include "pomdog/signals/event_queue.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::IOKit {
namespace {

void appendDeviceMatching(CFMutableArrayRef matcher, uint32_t page, uint32_t usage)
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

[[nodiscard]] float
normalizeAxisValue(IOHIDValueRef valueRef, const ThumbStickInfo& info)
{
    const auto value = IOHIDValueGetIntegerValue(valueRef);
    return static_cast<float>(info.invertDirection * ((value - info.minimum) * 2 - info.range)) / info.range;
}

} // namespace

void GamepadDevice::close()
{
    if (device != nullptr) {
        IOHIDDeviceClose(device, kIOHIDOptionsTypeNone);
        device = nullptr;
    }

    GamepadHelper::clearState(state);
    state.isConnected = false;

    GamepadCapabilities emptyCaps;
    std::swap(caps, emptyCaps);
}

void GamepadDevice::onDeviceInput(IOReturn result, void* sender, IOHIDValueRef valueRef)
{
    const auto element = IOHIDValueGetElement(valueRef);
    const auto usagePage = IOHIDElementGetUsagePage(element);
    const auto usage = IOHIDElementGetUsage(element);

    switch (IOHIDElementGetType(element)) {
    case kIOHIDElementTypeInput_Button: {
        const auto buttonIndex = static_cast<int>(usage) - 1;
        if (auto button = gamepad_mappings::getButton(state, mappings.buttons, buttonIndex); button != nullptr) {
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

                if (auto thumbStick = gamepad_mappings::getThumbStick(state, mapper.thumbStick); thumbStick != nullptr) {
                    (*thumbStick) = normalizeAxisValue(valueRef, thumbStickInfos[axisIndex]);
                }

                if (auto button = gamepad_mappings::getButton(state, mapper.positiveTrigger); button != nullptr) {
                    const auto value = normalizeAxisValue(valueRef, thumbStickInfos[axisIndex]);
                    constexpr float threshold = 0.05f;
                    (*button) = (value > threshold) ? ButtonState::Pressed : ButtonState::Released;
                }

                if (auto button = gamepad_mappings::getButton(state, mapper.negativeTrigger); button != nullptr) {
                    const auto value = normalizeAxisValue(valueRef, thumbStickInfos[axisIndex]);
                    constexpr float threshold = -0.05f;
                    (*button) = (value < threshold) ? ButtonState::Pressed : ButtonState::Released;
                }

                break;
            }
            case kHIDUsage_GD_Hatswitch: {
                state.dpad.up = ButtonState::Released;
                state.dpad.down = ButtonState::Released;
                state.dpad.left = ButtonState::Released;
                state.dpad.right = ButtonState::Released;

                switch (IOHIDValueGetIntegerValue(valueRef)) {
                case 0:
                    state.dpad.up = ButtonState::Pressed;
                    break;
                case 1:
                    state.dpad.up = ButtonState::Pressed;
                    state.dpad.right = ButtonState::Pressed;
                    break;
                case 2:
                    state.dpad.right = ButtonState::Pressed;
                    break;
                case 3:
                    state.dpad.right = ButtonState::Pressed;
                    state.dpad.down = ButtonState::Pressed;
                    break;
                case 4:
                    state.dpad.down = ButtonState::Pressed;
                    break;
                case 5:
                    state.dpad.down = ButtonState::Pressed;
                    state.dpad.left = ButtonState::Pressed;
                    break;
                case 6:
                    state.dpad.left = ButtonState::Pressed;
                    break;
                case 7:
                    state.dpad.up = ButtonState::Pressed;
                    state.dpad.left = ButtonState::Pressed;
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

GamepadIOKit::GamepadIOKit() = default;

std::unique_ptr<Error>
GamepadIOKit::initialize(const std::shared_ptr<EventQueue<SystemEvent>>& eventQueueIn)
{
    eventQueue_ = eventQueueIn;

    gamepads_[0].playerIndex = PlayerIndex::One;
    gamepads_[1].playerIndex = PlayerIndex::Two;
    gamepads_[2].playerIndex = PlayerIndex::Three;
    gamepads_[3].playerIndex = PlayerIndex::Four;

    hidManager_ = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (hidManager_ == nullptr) {
        return errors::make("IOHIDManagerCreate() failed");
    }

    auto deviceMatcher = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
    if (deviceMatcher == nullptr) {
        return errors::make("CFArrayCreateMutable() failed");
    }

    appendDeviceMatching(deviceMatcher, kHIDPage_GenericDesktop, kHIDUsage_GD_Joystick);
    appendDeviceMatching(deviceMatcher, kHIDPage_GenericDesktop, kHIDUsage_GD_GamePad);

    IOHIDManagerSetDeviceMatchingMultiple(hidManager_, deviceMatcher);
    CFRelease(deviceMatcher);

    IOHIDManagerRegisterDeviceMatchingCallback(
        hidManager_,
        [](void* context, IOReturn result, void* sender, IOHIDDeviceRef device) {
            auto c = reinterpret_cast<GamepadIOKit*>(context);
            POMDOG_ASSERT(c != nullptr);
            c->onDeviceAttached(result, sender, device);
        },
        this);
    IOHIDManagerRegisterDeviceRemovalCallback(
        hidManager_,
        [](void* context, IOReturn result, void* sender, IOHIDDeviceRef device) {
            auto c = reinterpret_cast<GamepadIOKit*>(context);
            POMDOG_ASSERT(c != nullptr);
            c->onDeviceDetached(result, sender, device);
        },
        this);

    IOHIDManagerScheduleWithRunLoop(hidManager_, CFRunLoopGetMain(), kCFRunLoopCommonModes);
    IOHIDManagerOpen(hidManager_, kIOHIDOptionsTypeNone);

    return nullptr;
}

GamepadIOKit::~GamepadIOKit()
{
    for (auto& device : gamepads_) {
        device.close();
    }

    if (hidManager_ != nullptr) {
        IOHIDManagerUnscheduleFromRunLoop(hidManager_, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
        IOHIDManagerClose(hidManager_, kIOHIDOptionsTypeNone);
        CFRelease(hidManager_);
        hidManager_ = nullptr;
    }
}

GamepadCapabilities
GamepadIOKit::getCapabilities(PlayerIndex playerIndex) const
{
    const auto index = GamepadHelper::toInt(playerIndex);
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(gamepads_.size()));
    return gamepads_[index].caps;
}

GamepadState
GamepadIOKit::getState(PlayerIndex playerIndex) const
{
    const auto index = GamepadHelper::toInt(playerIndex);
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(gamepads_.size()));
    return gamepads_[index].state;
}

void GamepadIOKit::onDeviceAttached(IOReturn result, void* sender, IOHIDDeviceRef device)
{
    const auto gamepad = std::find_if(std::begin(gamepads_), std::end(gamepads_), [](GamepadDevice& a) {
        return a.device == nullptr;
    });
    if (gamepad == std::end(gamepads_)) {
        return;
    }

    GamepadHelper::clearState(gamepad->state);
    gamepad->device = device;
    gamepad->state.isConnected = true;

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

    auto& uuid = gamepad->caps.deviceUUID;
    uuid.busType = busUSB;
    uuid.vendorID = static_cast<uint16_t>(vendor);
    uuid.productID = static_cast<uint16_t>(product);
    uuid.versionNumber = static_cast<uint16_t>(version);

    std::tie(gamepad->mappings, gamepad->caps.name) = gamepad_mappings::getMappings(uuid);
    if (gamepad->caps.name.empty()) {
        auto productKey = reinterpret_cast<CFStringRef>(IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductKey)));
        if (productKey != nullptr) {
            std::array<char, 256> buffer;
            std::fill(std::begin(buffer), std::end(buffer), 0);
            CFStringGetCString(productKey, buffer.data(), buffer.size(), kCFStringEncodingUTF8);
            gamepad->caps.name = buffer.data();
        }
    }

    auto elements = IOHIDDeviceCopyMatchingElements(device, nullptr, kIOHIDOptionsTypeNone);
    if (elements != nullptr) {
        for (int i = 0; i < CFArrayGetCount(elements); ++i) {
            auto element = reinterpret_cast<IOHIDElementRef>(const_cast<void*>(CFArrayGetValueAtIndex(elements, i)));
            if (CFGetTypeID(element) != IOHIDElementGetTypeID()) {
                continue;
            }

            const auto usagePage = IOHIDElementGetUsagePage(element);
            const auto usage = IOHIDElementGetUsage(element);

            switch (IOHIDElementGetType(element)) {
            case kIOHIDElementTypeInput_Button: {
                const auto buttonIndex = static_cast<int>(usage) - 1;
                if (auto hasButton = gamepad_mappings::hasButton(gamepad->caps, gamepad->mappings.buttons, buttonIndex); hasButton != nullptr) {
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

                        if (auto hasThumbStick = gamepad_mappings::hasThumbStick(gamepad->caps, mapper.thumbStick); hasThumbStick != nullptr) {
                            (*hasThumbStick) = true;
                        }
                        if (auto hasButton = gamepad_mappings::hasButton(gamepad->caps, mapper.positiveTrigger); hasButton != nullptr) {
                            (*hasButton) = true;
                        }
                        if (auto hasButton = gamepad_mappings::hasButton(gamepad->caps, mapper.negativeTrigger); hasButton != nullptr) {
                            (*hasButton) = true;
                        }

                        POMDOG_ASSERT(axisIndex >= 0);
                        POMDOG_ASSERT(axisIndex < static_cast<int>(gamepad->thumbStickInfos.size()));

                        const auto minimum = static_cast<int32_t>(IOHIDElementGetLogicalMin(element));
                        const auto maximum = static_cast<int32_t>(IOHIDElementGetLogicalMax(element));
                        auto& info = gamepad->thumbStickInfos[axisIndex];
                        info.minimum = minimum;
                        info.range = std::max(1, maximum - minimum);
                        switch (mapper.thumbStick) {
                        case ThumbStickKind::LeftStickY:
                        case ThumbStickKind::RightStickY:
                            // Set to -1 to reverse the Y axis (vertical)
                            info.invertDirection = -1;
                            break;
                        default:
                            info.invertDirection = 1;
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

    SystemEvent event;
    event.kind = SystemEventKind::GamepadDisconnectedEvent;
    event.data = GamepadEvent{
        .playerIndex = gamepad->playerIndex,
        .capabilities = gamepad->caps,
    };
    eventQueue_->enqueue(std::move(event));

    IOHIDDeviceOpen(device, kIOHIDOptionsTypeNone);
    IOHIDDeviceScheduleWithRunLoop(device, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
    IOHIDDeviceRegisterInputValueCallback(
        device,
        [](void* context, IOReturn resultIn, void* senderIn, IOHIDValueRef value) {
            auto c = reinterpret_cast<GamepadDevice*>(context);
            POMDOG_ASSERT(c != nullptr);
            c->onDeviceInput(resultIn, senderIn, value);
        },
        &(*gamepad));
}

void GamepadIOKit::onDeviceDetached(IOReturn result, void* sender, IOHIDDeviceRef device)
{
    auto gamepad = std::find_if(std::begin(gamepads_), std::end(gamepads_), [&](GamepadDevice& a) {
        return a.device == device;
    });
    if (gamepad == std::end(gamepads_)) {
        // NOTE: The device was not found.
        return;
    }

    auto caps = gamepad->caps;
    gamepad->close();

    SystemEvent event;
    event.kind = SystemEventKind::GamepadDisconnectedEvent;
    event.data = GamepadEvent{
        .playerIndex = gamepad->playerIndex,
        .capabilities = std::move(caps),
    };
    eventQueue_->enqueue(std::move(event));
}

void GamepadIOKit::handleEvent(const SystemEvent& event)
{
    switch (event.kind) {
    case SystemEventKind::GamepadConnectedEvent: {
        const auto& e = std::get<GamepadEvent>(event.data);
        Connected(e.playerIndex, e.capabilities);
        break;
    }
    case SystemEventKind::GamepadDisconnectedEvent: {
        const auto& e = std::get<GamepadEvent>(event.data);
        Disconnected(e.playerIndex, e.capabilities);
        break;
    }
    default:
        break;
    }
}

} // namespace pomdog::detail::IOKit
