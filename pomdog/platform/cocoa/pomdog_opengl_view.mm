// Copyright mogemimi. Distributed under the MIT license.

#import "pomdog/platform/cocoa/pomdog_opengl_view.hpp"
#include "pomdog/application/system_events.hpp"
#include "pomdog/input/button_state.hpp"
#include "pomdog/input/keys.hpp"
#include "pomdog/math/point2d.hpp"
#include "pomdog/platform/cocoa/game_host_cocoa.hpp"
#include "pomdog/platform/cocoa/opengl_context_cocoa.hpp"
#include "pomdog/utility/assert.hpp"
#include <memory>
#include <utility>

using pomdog::detail::cocoa::OpenGLContextCocoa;
using pomdog::detail::InputKeyEvent;
using pomdog::detail::MouseButtonCocoaEvent;
using pomdog::detail::MouseButtonState;
using pomdog::detail::MousePositionEvent;
using pomdog::detail::SystemEvent;
using pomdog::detail::SystemEventKind;
using pomdog::EventQueue;
using pomdog::KeyState;
using pomdog::Keys;
using pomdog::MouseButtons;
using pomdog::Point2D;

namespace {

Point2D ToPoint2D(const NSPoint& point)
{
    // NOTE: Convert from cartesian coordinates to screen coordinate system.
    // FIXME: Avoid using magic number `-2`
    return Point2D(point.x - 2, point.y - 2);
}

Keys TranslateKey(std::uint16_t keyCode)
{
    constexpr std::array<Keys, 127> keyTable = {
        Keys::A,              // 0x00: kVK_ANSI_A
        Keys::S,              // 0x01: kVK_ANSI_S
        Keys::D,              // 0x02: kVK_ANSI_D
        Keys::F,              // 0x03: kVK_ANSI_F
        Keys::H,              // 0x04: kVK_ANSI_H
        Keys::G,              // 0x05: kVK_ANSI_G
        Keys::Z,              // 0x06: kVK_ANSI_Z
        Keys::X,              // 0x07: kVK_ANSI_X
        Keys::C,              // 0x08: kVK_ANSI_C
        Keys::V,              // 0x09: kVK_ANSI_V
        Keys::Unknown,        // 0x0A: kVK_ISO_Section
        Keys::B,              // 0x0B: kVK_ANSI_B
        Keys::Q,              // 0x0C: kVK_ANSI_Q
        Keys::W,              // 0x0D: kVK_ANSI_W
        Keys::E,              // 0x0E: kVK_ANSI_E
        Keys::R,              // 0x0F: kVK_ANSI_R
        Keys::Y,              // 0x10: kVK_ANSI_Y
        Keys::T,              // 0x11: kVK_ANSI_T
        Keys::Alpha1,         // 0x12: kVK_ANSI_1
        Keys::Alpha2,         // 0x13: kVK_ANSI_2
        Keys::Alpha3,         // 0x14: kVK_ANSI_3
        Keys::Alpha4,         // 0x15: kVK_ANSI_4
        Keys::Alpha6,         // 0x16: kVK_ANSI_6
        Keys::Alpha5,         // 0x17: kVK_ANSI_5
        Keys::Equals,         // 0x18: kVK_ANSI_Equal
        Keys::Alpha9,         // 0x19: kVK_ANSI_9
        Keys::Alpha7,         // 0x1A: kVK_ANSI_7
        Keys::Minus,          // 0x1B: kVK_ANSI_Minus
        Keys::Alpha8,         // 0x1C: kVK_ANSI_8
        Keys::Alpha0,         // 0x1D: kVK_ANSI_0
        Keys::CloseBracket,   // 0x1E: kVK_ANSI_RightBracket
        Keys::O,              // 0x1F: kVK_ANSI_O
        Keys::U,              // 0x20: kVK_ANSI_U
        Keys::OpenBracket,    // 0x21: kVK_ANSI_LeftBracket
        Keys::I,              // 0x22: kVK_ANSI_I
        Keys::P,              // 0x23: kVK_ANSI_P
        Keys::Enter,          // 0x24: kVK_Return
        Keys::L,              // 0x25: kVK_ANSI_L
        Keys::J,              // 0x26: kVK_ANSI_J
        Keys::Quote,          // 0x27: kVK_ANSI_Quote
        Keys::K,              // 0x28: kVK_ANSI_K
        Keys::Semicolon,      // 0x29: kVK_ANSI_Semicolon
        Keys::Backslash,      // 0x2A: kVK_ANSI_Backslash
        Keys::Comma,          // 0x2B: kVK_ANSI_Comma
        Keys::Slash,          // 0x2C: kVK_ANSI_Slash
        Keys::N,              // 0x2D: kVK_ANSI_N
        Keys::M,              // 0x2E: kVK_ANSI_M
        Keys::Period,         // 0x2F: kVK_ANSI_Period
        Keys::Tab,            // 0x30: kVK_Tab
        Keys::Space,          // 0x31: kVK_Space
        Keys::BackQuate,      // 0x32: kVK_ANSI_Grave
        Keys::Delete,         // 0x33: kVK_Delete
        Keys::Unknown,        // 0x34: Enter key on keypad of PowerBook
        Keys::Escape,         // 0x35: kVK_Escape
        Keys::RightSuper,     // 0x36: right-command key
        Keys::LeftSuper,      // 0x37: kVK_Command
        Keys::LeftShift,      // 0x38: kVK_Shift
        Keys::CapsLock,       // 0x39: kVK_CapsLock
        Keys::LeftAlt,        // 0x3A: kVK_Option
        Keys::LeftControl,    // 0x3B: kVK_Control
        Keys::RightShift,     // 0x3C: kVK_RightShift
        Keys::RightAlt,       // 0x3D: kVK_RightOption
        Keys::RightControl,   // 0x3E: kVK_RightControl
        Keys::Unknown,        // 0x3F: kVK_Function
        Keys::F17,            // 0x40: kVK_F17
        Keys::KeypadDecimal,  // 0x41: kVK_ANSI_KeypadDecimal
        Keys::Unknown,        // 0x42:
        Keys::KeypadMultiply, // 0x43: kVK_ANSI_KeypadMultiply
        Keys::Unknown,        // 0x44:
        Keys::KeypadAdd,      // 0x45: kVK_ANSI_KeypadPlus
        Keys::Unknown,        // 0x46:
        Keys::Clear,          // 0x47: kVK_ANSI_KeypadClear
        Keys::VolumeUp,       // 0x48: kVK_VolumeUp
        Keys::VolumeDown,     // 0x49: kVK_VolumeDown
        Keys::VolumeMute,     // 0x4A: kVK_Mute
        Keys::KeypadDivide,   // 0x4B: kVK_ANSI_KeypadDivide
        Keys::Enter,          // 0x4C: kVK_ANSI_KeypadEnter
        Keys::Unknown,        // 0x4D:
        Keys::KeypadSubtract, // 0x4E: kVK_ANSI_KeypadMinus
        Keys::F18,            // 0x4F: kVK_F18
        Keys::F19,            // 0x50: kVK_F19
        Keys::Equals,         // 0x51: kVK_ANSI_KeypadEquals
        Keys::Keypad0,        // 0x52: kVK_ANSI_Keypad0
        Keys::Keypad1,        // 0x53: kVK_ANSI_Keypad1
        Keys::Keypad2,        // 0x54: kVK_ANSI_Keypad2
        Keys::Keypad3,        // 0x55: kVK_ANSI_Keypad3
        Keys::Keypad4,        // 0x56: kVK_ANSI_Keypad4
        Keys::Keypad5,        // 0x57: kVK_ANSI_Keypad5
        Keys::Keypad6,        // 0x58: kVK_ANSI_Keypad6
        Keys::Keypad7,        // 0x59: kVK_ANSI_Keypad7
        Keys::F20,            // 0x5A: kVK_F20
        Keys::Keypad8,        // 0x5B: kVK_ANSI_Keypad8
        Keys::Keypad9,        // 0x5C: kVK_ANSI_Keypad9
        Keys::Backslash,      // 0x5D: kVK_JIS_Yen
        Keys::Unknown,        // 0x5E: kVK_JIS_Underscore
        Keys::Comma,          // 0x5F: kVK_JIS_KeypadComma
        Keys::F5,             // 0x60: kVK_F5
        Keys::F6,             // 0x61: kVK_F6
        Keys::F7,             // 0x62: kVK_F7
        Keys::F3,             // 0x63: kVK_F3
        Keys::F8,             // 0x64: kVK_F8
        Keys::F9,             // 0x65: kVK_F9
        Keys::Unknown,        // 0x66: kVK_JIS_Eisu
        Keys::F11,            // 0x67: kVK_F11
        Keys::KanaMode,       // 0x68: kVK_JIS_Kana
        Keys::F13,            // 0x69: kVK_F13
        Keys::F16,            // 0x6A: kVK_F16
        Keys::F14,            // 0x6B: kVK_F14
        Keys::Unknown,        // 0x6C:
        Keys::F10,            // 0x6D: kVK_F10
        Keys::Unknown,        // 0x6E:
        Keys::F12,            // 0x6F: kVK_F12
        Keys::Unknown,        // 0x70:
        Keys::F15,            // 0x71: kVK_F15
        Keys::Help,           // 0x72: kVK_Help
        Keys::Home,           // 0x73: kVK_Home
        Keys::PageUp,         // 0x74: kVK_PageUp
        Keys::Delete,         // 0x75: kVK_ForwardDelete
        Keys::F4,             // 0x76: kVK_F4
        Keys::End,            // 0x77: kVK_End
        Keys::F2,             // 0x78: kVK_F2
        Keys::PageDown,       // 0x79: kVK_PageDown
        Keys::F1,             // 0x7A: kVK_F1
        Keys::LeftArrow,      // 0x7B: kVK_LeftArrow
        Keys::RightArrow,     // 0x7C: kVK_RightArrow
        Keys::DownArrow,      // 0x7D: kVK_DownArrow
        Keys::UpArrow,        // 0x7E: kVK_UpArrow
    };

    if (keyCode < keyTable.size()) {
        return keyTable[keyCode];
    }
    return Keys::Unknown;
}

NSUInteger TranslateKeyToModifierFlag(Keys key)
{
    switch (key) {
    case Keys::CapsLock:
        return NSEventModifierFlagCapsLock;
    case Keys::LeftShift:
    case Keys::RightShift:
        return NSEventModifierFlagShift;
    case Keys::LeftControl:
    case Keys::RightControl:
        return NSEventModifierFlagControl;
    case Keys::LeftAlt:
    case Keys::RightAlt:
        return NSEventModifierFlagOption;
    case Keys::LeftSuper:
    case Keys::RightSuper:
        return NSEventModifierFlagCommand;
    default:
        break;
    }
    return 0;
}

} // namespace

@implementation PomdogOpenGLView {
@private
    std::function<void()> renderCallback;
    std::function<void(bool)> resizingCallback;
    std::shared_ptr<EventQueue<SystemEvent>> eventQueue;
    std::shared_ptr<OpenGLContextCocoa> openGLContext;
    NSTrackingRectTag trackingRect;
    NSCursor* cursor;
    BOOL wasAcceptingMouseEvents;
}

- (instancetype)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if (self) {
        wasAcceptingMouseEvents = NO;
        cursor = nullptr;
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder*)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        wasAcceptingMouseEvents = NO;
    }
    return self;
}

- (void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];
    if (renderCallback) {
        renderCallback();
    }
}

// MARK: - View Event Handling

- (void)viewWillStartLiveResize
{
    if (eventQueue) {
        eventQueue->Enqueue(SystemEvent{.Kind = SystemEventKind::ViewWillStartLiveResizeEvent});
    }

    if (resizingCallback) {
        resizingCallback(true);
    }
}

- (void)viewDidEndLiveResize
{
    if (eventQueue) {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::ViewDidEndLiveResizeEvent,
        });
    }

    if (resizingCallback) {
        resizingCallback(false);
    }
}

- (void)viewDidMoveToWindow
{
    [super viewDidMoveToWindow];

    if ([self window] == nil) {
        auto nativeContext = openGLContext->GetNativeOpenGLContext();
        [nativeContext clearDrawable];
    }

    trackingRect = [self addTrackingRect:[self bounds] owner:self userData:nullptr assumeInside:NO];
}

- (void)viewWillMoveToWindow:(NSWindow*)newWindow
{
    [super viewWillMoveToWindow:newWindow];

    if ([self window] && trackingRect) {
        [self removeTrackingRect:trackingRect];
    }
}

// MARK: - Mouse-Tracking and Cursor

- (void)setFrame:(NSRect)frame
{
    [super setFrame:frame];
    [self removeTrackingRect:trackingRect];
    trackingRect = [self addTrackingRect:[self bounds] owner:self userData:nullptr assumeInside:NO];
}

- (void)setBounds:(NSRect)bounds
{
    [super setBounds:bounds];
    [self removeTrackingRect:trackingRect];
    trackingRect = [self addTrackingRect:[self bounds] owner:self userData:nullptr assumeInside:NO];
}

// MARK: - Getter/Setter

- (void)setEventQueue:(std::shared_ptr<EventQueue<SystemEvent>>)eventQueueIn
{
    eventQueue = eventQueueIn;
}

- (void)setOpenGLContext:(std::shared_ptr<pomdog::detail::cocoa::OpenGLContextCocoa>)openGLContextIn
{
    openGLContext = openGLContextIn;
}

- (void)setRenderCallback:(std::function<void()>)callback
{
    renderCallback = callback;
}

- (void)setResizingCallback:(std::function<void(bool)>)callback
{
    resizingCallback = callback;
}

// MARK: - Settings

- (BOOL)isFlipped
{
    return TRUE;
}

// MARK: - Mouse Event Handling

- (void)setMouseCursor:(NSCursor*)cursorIn
{
    cursor = cursorIn;
}

- (void)cursorUpdate:(NSEvent*)event
{
    if (cursor != nullptr) {
        [cursor set];
    }
}

- (void)mouseEntered:(NSEvent*)theEvent
{
    if (!eventQueue) {
        return;
    }

    wasAcceptingMouseEvents = [[self window] acceptsMouseMovedEvents];
    [[self window] setAcceptsMouseMovedEvents:YES];
    [[self window] makeFirstResponder:self];

    NSPoint locationInView = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    eventQueue->Enqueue(SystemEvent{
        .Kind = SystemEventKind::MouseEnteredEvent,
        .Data = MousePositionEvent{
            .Position = ToPoint2D(locationInView),
        },
    });
}

- (void)mouseMoved:(NSEvent*)theEvent
{
    if (eventQueue) {
        NSPoint locationInView = [self convertPoint:[theEvent locationInWindow] fromView:nil];
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseMovedEvent,
            .Data = MousePositionEvent{
                .Position = ToPoint2D(locationInView),
            },
        });
    }
}

- (void)mouseExited:(NSEvent*)theEvent
{
    [[self window] setAcceptsMouseMovedEvents:wasAcceptingMouseEvents];

    if (eventQueue) {
        NSPoint locationInView = [self convertPoint:[theEvent locationInWindow] fromView:nil];
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseExitedEvent,
            .Data = MousePositionEvent{
                .Position = ToPoint2D(locationInView),
            },
        });
    }
}

- (void)mouseDown:(NSEvent*)theEvent
{
    if (eventQueue) {
        NSPoint locationInView = [self convertPoint:[theEvent locationInWindow] fromView:nil];
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseButtonEvent,
            .Data = MouseButtonCocoaEvent{
                .Position = ToPoint2D(locationInView),
                .Button = MouseButtons::Left,
                .State = MouseButtonState::Down,
            },
        });
    }
}

- (void)mouseDragged:(NSEvent*)theEvent
{
    if (eventQueue) {
        NSPoint locationInView = [self convertPoint:[theEvent locationInWindow] fromView:nil];
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseButtonEvent,
            .Data = MouseButtonCocoaEvent{
                .Position = ToPoint2D(locationInView),
                .Button = MouseButtons::Left,
                .State = MouseButtonState::Dragged,
            },
        });
    }
}

- (void)mouseUp:(NSEvent*)theEvent
{
    if (eventQueue) {
        NSPoint locationInView = [self convertPoint:[theEvent locationInWindow] fromView:nil];
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseButtonEvent,
            .Data = MouseButtonCocoaEvent{
                .Position = ToPoint2D(locationInView),
                .Button = MouseButtons::Left,
                .State = MouseButtonState::Up,
            },
        });
    }
}

- (void)rightMouseDown:(NSEvent*)theEvent
{
    if (eventQueue) {
        NSPoint locationInView = [self convertPoint:[theEvent locationInWindow] fromView:nil];
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseButtonEvent,
            .Data = MouseButtonCocoaEvent{
                .Position = ToPoint2D(locationInView),
                .Button = MouseButtons::Right,
                .State = MouseButtonState::Down,
            },
        });
    }
}

- (void)rightMouseDragged:(NSEvent*)theEvent
{
    if (eventQueue) {
        NSPoint locationInView = [self convertPoint:[theEvent locationInWindow] fromView:nil];
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseButtonEvent,
            .Data = MouseButtonCocoaEvent{
                .Position = ToPoint2D(locationInView),
                .Button = MouseButtons::Right,
                .State = MouseButtonState::Dragged,
            },
        });
    }
}

- (void)rightMouseUp:(NSEvent*)theEvent
{
    if (eventQueue) {
        NSPoint locationInView = [self convertPoint:[theEvent locationInWindow] fromView:nil];
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::MouseButtonEvent,
            .Data = MouseButtonCocoaEvent{
                .Position = ToPoint2D(locationInView),
                .Button = MouseButtons::Right,
                .State = MouseButtonState::Up,
            },
        });
    }
}

- (void)otherMouseDown:(NSEvent*)theEvent
{
    if (!eventQueue) {
        return;
    }

    MouseButtonCocoaEvent event;
    NSInteger buttonNumber = [theEvent buttonNumber];

    if (buttonNumber == 2) {
        event.Button = MouseButtons::Middle;
    }
    else if (buttonNumber == 3) {
        event.Button = MouseButtons::XButton1;
    }
    else if (buttonNumber == 4) {
        event.Button = MouseButtons::XButton2;
    }

    NSPoint locationInView = [self convertPoint:[theEvent locationInWindow] fromView:nil];

    event.State = MouseButtonState::Down;
    event.Position = ToPoint2D(locationInView);

    eventQueue->Enqueue(SystemEvent{
        .Kind = SystemEventKind::MouseButtonEvent,
        .Data = std::move(event),
    });
}

- (void)otherMouseDragged:(NSEvent*)theEvent
{
    if (!eventQueue) {
        return;
    }

    MouseButtonCocoaEvent event;
    NSInteger buttonNumber = [theEvent buttonNumber];

    if (buttonNumber == 2) {
        event.Button = MouseButtons::Middle;
    }
    else if (buttonNumber == 3) {
        event.Button = MouseButtons::XButton1;
    }
    else if (buttonNumber == 4) {
        event.Button = MouseButtons::XButton2;
    }

    NSPoint locationInView = [self convertPoint:[theEvent locationInWindow] fromView:nil];

    event.State = MouseButtonState::Dragged;
    event.Position = ToPoint2D(locationInView);

    eventQueue->Enqueue(SystemEvent{
        .Kind = SystemEventKind::MouseButtonEvent,
        .Data = std::move(event),
    });
}

- (void)otherMouseUp:(NSEvent*)theEvent
{
    if (!eventQueue) {
        return;
    }

    MouseButtonCocoaEvent event;
    NSInteger buttonNumber = [theEvent buttonNumber];

    if (buttonNumber == 2) {
        event.Button = MouseButtons::Middle;
    }
    else if (buttonNumber == 3) {
        event.Button = MouseButtons::XButton1;
    }
    else if (buttonNumber == 4) {
        event.Button = MouseButtons::XButton2;
    }

    NSPoint locationInView = [self convertPoint:[theEvent locationInWindow] fromView:nil];

    event.State = MouseButtonState::Up;
    event.Position = ToPoint2D(locationInView);

    eventQueue->Enqueue(SystemEvent{
        .Kind = SystemEventKind::MouseButtonEvent,
        .Data = std::move(event),
    });
}

- (void)scrollWheel:(NSEvent*)theEvent
{
    if (!eventQueue) {
        return;
    }

    pomdog::detail::ScrollWheelCocoaEvent event;

    double scrollingDeltaY = static_cast<double>([theEvent scrollingDeltaY]);
    if ([theEvent hasPreciseScrollingDeltas]) {
        event.ScrollingDeltaY = scrollingDeltaY * 0.1;
    }
    else {
        event.ScrollingDeltaY = scrollingDeltaY;
    }

    eventQueue->Enqueue(SystemEvent{
        .Kind = SystemEventKind::ScrollWheelEvent,
        .Data = std::move(event),
    });
}

// MARK: - Keyboard Event Handling

- (void)keyDown:(NSEvent*)theEvent
{
    auto key = TranslateKey([theEvent keyCode]);
    if (key != pomdog::Keys::Unknown) {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::InputKeyEvent,
            .Data = InputKeyEvent{
                .State = KeyState::Down,
                .Key = key,
            },
        });
    }

    using pomdog::detail::InputTextEvent;
    std::string text = [[theEvent characters] UTF8String];

    eventQueue->Enqueue(SystemEvent{
        .Kind = SystemEventKind::InputTextEvent,
        .Data = InputTextEvent{
            .Text = std::move(text),
        },
    });
}

- (void)keyUp:(NSEvent*)theEvent
{
    auto key = TranslateKey([theEvent keyCode]);
    if (key != pomdog::Keys::Unknown) {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::InputKeyEvent,
            .Data = InputKeyEvent{
                .State = KeyState::Up,
                .Key = key,
            },
        });
    }
}

- (void)flagsChanged:(NSEvent*)event
{
    const auto key = TranslateKey([event keyCode]);
    if (key == pomdog::Keys::Unknown) {
        return;
    }

    const auto modifierFlags = [event modifierFlags] & NSEventModifierFlagDeviceIndependentFlagsMask;
    const auto flag = TranslateKeyToModifierFlag(key);
    if ((flag & modifierFlags) != 0) {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::InputKeyEvent,
            .Data = InputKeyEvent{
                .State = KeyState::Down,
                .Key = key,
            },
        });
    }
    else {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::InputKeyEvent,
            .Data = InputKeyEvent{
                .State = KeyState::Up,
                .Key = key,
            },
        });
    }
}

@end
