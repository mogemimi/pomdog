// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#import "PomdogOpenGLView.hpp"
#include "GameHostCocoa.hpp"
#include "OpenGLContextCocoa.hpp"
#include "../Application/SystemEvents.hpp"
#include "Pomdog/Input/ButtonState.hpp"
#include "Pomdog/Input/Keys.hpp"
#include "Pomdog/Math/Point2D.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <memory>
#include <utility>

using Pomdog::Detail::Cocoa::OpenGLContextCocoa;
using Pomdog::Detail::InputKeyEvent;
using Pomdog::Detail::MouseButtonCocoaEvent;
using Pomdog::Detail::MouseButtonState;
using Pomdog::Detail::MousePositionEvent;
using Pomdog::Detail::SystemEvent;
using Pomdog::Detail::SystemEventKind;
using Pomdog::EventQueue;
using Pomdog::KeyState;
using Pomdog::Keys;
using Pomdog::MouseButtons;
using Pomdog::Point2D;

namespace {

Point2D ToPoint2D(const NSPoint& point)
{
    // NOTE: Convert from cartesian coordinates to screen coordinate system.
    // FIXME: Avoid using magic number `-2`
    return Point2D(point.x - 2, point.y - 2);
}

Keys TranslateKey(std::uint16_t keyCode)
{
    constexpr Keys keyTable[127] = {
        /* 0x00 */ Keys::A, // kVK_ANSI_A
        /* 0x01 */ Keys::S, // kVK_ANSI_S
        /* 0x02 */ Keys::D, // kVK_ANSI_D
        /* 0x03 */ Keys::F, // kVK_ANSI_F
        /* 0x04 */ Keys::H, // kVK_ANSI_H
        /* 0x05 */ Keys::G, // kVK_ANSI_G
        /* 0x06 */ Keys::Z, // kVK_ANSI_Z
        /* 0x07 */ Keys::X, // kVK_ANSI_X
        /* 0x08 */ Keys::C, // kVK_ANSI_C
        /* 0x09 */ Keys::V, // kVK_ANSI_V
        /* 0x0A */ Keys::Unknown, // kVK_ISO_Section
        /* 0x0B */ Keys::B, // kVK_ANSI_B
        /* 0x0C */ Keys::Q, // kVK_ANSI_Q
        /* 0x0D */ Keys::W, // kVK_ANSI_W
        /* 0x0E */ Keys::E, // kVK_ANSI_E
        /* 0x0F */ Keys::R, // kVK_ANSI_R
        /* 0x10 */ Keys::Y, // kVK_ANSI_Y
        /* 0x11 */ Keys::T, // kVK_ANSI_T
        /* 0x12 */ Keys::Alpha1, // kVK_ANSI_1
        /* 0x13 */ Keys::Alpha2, // kVK_ANSI_2
        /* 0x14 */ Keys::Alpha3, // kVK_ANSI_3
        /* 0x15 */ Keys::Alpha4, // kVK_ANSI_4
        /* 0x16 */ Keys::Alpha6, // kVK_ANSI_6
        /* 0x17 */ Keys::Alpha5, // kVK_ANSI_5
        /* 0x18 */ Keys::Equals, // kVK_ANSI_Equal
        /* 0x19 */ Keys::Alpha9, // kVK_ANSI_9
        /* 0x1A */ Keys::Alpha7, // kVK_ANSI_7
        /* 0x1B */ Keys::Minus, // kVK_ANSI_Minus
        /* 0x1C */ Keys::Alpha8, // kVK_ANSI_8
        /* 0x1D */ Keys::Alpha0, // kVK_ANSI_0
        /* 0x1E */ Keys::CloseBracket, // kVK_ANSI_RightBracket
        /* 0x1F */ Keys::O, // kVK_ANSI_O
        /* 0x20 */ Keys::U, // kVK_ANSI_U
        /* 0x21 */ Keys::OpenBracket, // kVK_ANSI_LeftBracket
        /* 0x22 */ Keys::I, // kVK_ANSI_I
        /* 0x23 */ Keys::P, // kVK_ANSI_P
        /* 0x24 */ Keys::Enter, // kVK_Return
        /* 0x25 */ Keys::L, // kVK_ANSI_L
        /* 0x26 */ Keys::J, // kVK_ANSI_J
        /* 0x27 */ Keys::Quote, // kVK_ANSI_Quote
        /* 0x28 */ Keys::K, // kVK_ANSI_K
        /* 0x29 */ Keys::Semicolon, // kVK_ANSI_Semicolon
        /* 0x2A */ Keys::Backslash, // kVK_ANSI_Backslash
        /* 0x2B */ Keys::Comma, // kVK_ANSI_Comma
        /* 0x2C */ Keys::Slash, // kVK_ANSI_Slash
        /* 0x2D */ Keys::N, // kVK_ANSI_N
        /* 0x2E */ Keys::M, // kVK_ANSI_M
        /* 0x2F */ Keys::Period, // kVK_ANSI_Period
        /* 0x30 */ Keys::Tab, // kVK_Tab
        /* 0x31 */ Keys::Space, // kVK_Space
        /* 0x32 */ Keys::AccentGrave, // kVK_ANSI_Grave
        /* 0x33 */ Keys::Delete, // kVK_Delete
        /* 0x34 */ Keys::Unknown,
        /* 0x35 */ Keys::Escape, // kVK_Escape
        /* 0x36 */ Keys::RightApple, // NOTE: maybe
        /* 0x37 */ Keys::LeftApple, // kVK_Command
        /* 0x38 */ Keys::LeftShift, // kVK_Shift
        /* 0x39 */ Keys::CapsLock, // kVK_CapsLock
        /* 0x3A */ Keys::LeftAlt, // kVK_Option
        /* 0x3B */ Keys::LeftControl, // kVK_Control
        /* 0x3C */ Keys::RightShift, // kVK_RightShift
        /* 0x3D */ Keys::RightAlt, // kVK_RightOption
        /* 0x3E */ Keys::RightControl, // kVK_RightControl
        /* 0x3F */ Keys::Unknown, // kVK_Function
        /* 0x40 */ Keys::Unknown, // kVK_F17
        /* 0x41 */ Keys::KeypadDecimal, // kVK_ANSI_KeypadDecimal
        /* 0x42 */ Keys::Unknown,
        /* 0x43 */ Keys::KeypadMultiply, // kVK_ANSI_KeypadMultiply
        /* 0x44 */ Keys::Unknown,
        /* 0x45 */ Keys::KeypadAdd, // kVK_ANSI_KeypadPlus
        /* 0x46 */ Keys::Unknown,
        /* 0x47 */ Keys::Clear, // kVK_ANSI_KeypadClear
        /* 0x48 */ Keys::Unknown, // kVK_VolumeUp
        /* 0x49 */ Keys::Unknown, // kVK_VolumeDown
        /* 0x4A */ Keys::Unknown, // kVK_Mute
        /* 0x4B */ Keys::KeypadDivide, // kVK_ANSI_KeypadDivide
        /* 0x4C */ Keys::Enter, // kVK_ANSI_KeypadEnter
        /* 0x4D */ Keys::Unknown,
        /* 0x4E */ Keys::KeypadSubtract, // kVK_ANSI_KeypadMinus
        /* 0x4F */ Keys::Unknown, // kVK_F18
        /* 0x50 */ Keys::Unknown, // kVK_F19
        /* 0x51 */ Keys::Equals, // kVK_ANSI_KeypadEquals
        /* 0x52 */ Keys::Keypad0, // kVK_ANSI_Keypad0
        /* 0x53 */ Keys::Keypad1, // kVK_ANSI_Keypad1
        /* 0x54 */ Keys::Keypad2, // kVK_ANSI_Keypad2
        /* 0x55 */ Keys::Keypad3, // kVK_ANSI_Keypad3
        /* 0x56 */ Keys::Keypad4, // kVK_ANSI_Keypad4
        /* 0x57 */ Keys::Keypad5, // kVK_ANSI_Keypad5
        /* 0x58 */ Keys::Keypad6, // kVK_ANSI_Keypad6
        /* 0x59 */ Keys::Keypad7, // kVK_ANSI_Keypad7
        /* 0x5A */ Keys::Unknown, // kVK_F20
        /* 0x5B */ Keys::Keypad8, // kVK_ANSI_Keypad8
        /* 0x5C */ Keys::Keypad9, // kVK_ANSI_Keypad9
        /* 0x5D */ Keys::Backslash, // kVK_JIS_Yen
        /* 0x5E */ Keys::Unknown, // kVK_JIS_Underscore
        /* 0x5F */ Keys::Comma, // kVK_JIS_KeypadComma
        /* 0x60 */ Keys::F5, // kVK_F5
        /* 0x61 */ Keys::F6, // kVK_F6
        /* 0x62 */ Keys::F7, // kVK_F7
        /* 0x63 */ Keys::F3, // kVK_F3
        /* 0x64 */ Keys::F8, // kVK_F8
        /* 0x65 */ Keys::F9, // kVK_F9
        /* 0x66 */ Keys::Unknown, // kVK_JIS_Eisu
        /* 0x67 */ Keys::F11, // kVK_F11
        /* 0x68 */ Keys::KanaMode, // kVK_JIS_Kana
        /* 0x69 */ Keys::F13, // kVK_F13
        /* 0x6A */ Keys::Unknown, // kVK_F16
        /* 0x6B */ Keys::F14, // kVK_F14
        /* 0x6C */ Keys::Unknown,
        /* 0x6D */ Keys::F10, // kVK_F10
        /* 0x6E */ Keys::Unknown,
        /* 0x6F */ Keys::F12, // kVK_F12
        /* 0x70 */ Keys::Unknown,
        /* 0x71 */ Keys::F15, // kVK_F15
        /* 0x72 */ Keys::Help, // kVK_Help
        /* 0x73 */ Keys::Home, // kVK_Home
        /* 0x74 */ Keys::PageUp, // kVK_PageUp
        /* 0x75 */ Keys::Delete, // kVK_ForwardDelete
        /* 0x76 */ Keys::F4, // kVK_F4
        /* 0x77 */ Keys::End, // kVK_End
        /* 0x78 */ Keys::F2, // kVK_F2
        /* 0x79 */ Keys::PageDown, // kVK_PageDown
        /* 0x7A */ Keys::F1, // kVK_F1
        /* 0x7B */ Keys::LeftArrow, // kVK_LeftArrow
        /* 0x7C */ Keys::RightArrow, // kVK_RightArrow
        /* 0x7D */ Keys::DownArrow, // kVK_DownArrow
        /* 0x7E */ Keys::UpArrow, // kVK_UpArrow
    };

    if (keyCode < sizeof(keyTable)) {
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
    case Keys::LeftApple:
    case Keys::RightApple:
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

- (void)setOpenGLContext:(std::shared_ptr<Pomdog::Detail::Cocoa::OpenGLContextCocoa>)openGLContextIn
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

- (void)cursorUpdate:(NSEvent *)event
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

    Pomdog::Detail::ScrollWheelCocoaEvent event;

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
    if (key != Pomdog::Keys::Unknown) {
        eventQueue->Enqueue(SystemEvent{
            .Kind = SystemEventKind::InputKeyEvent,
            .Data = InputKeyEvent{
                .State = KeyState::Down,
                .Key = key,
            },
        });
    }

    using Pomdog::Detail::InputTextEvent;
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
    if (key != Pomdog::Keys::Unknown) {
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
    if (key == Pomdog::Keys::Unknown) {
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
