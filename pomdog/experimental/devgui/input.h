// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/text_input/text_input_event.h"
#include "pomdog/math/vector2.h"
#include "pomdog/memory/raw_ptr.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Keyboard;
class Mouse;
} // namespace pomdog

namespace pomdog::textinput {
class TextInputService;
} // namespace pomdog::textinput

namespace pomdog::devgui {

/// Identifies a key that devgui reads for text editing and for closing popups.
enum class EditKey : u8 {
    LeftArrow,
    RightArrow,
    UpArrow,
    DownArrow,
    Home,
    End,
    Backspace,
    Delete,
    Enter,
    Escape,
    Tab,

    /// The A key, which selects all text with the shortcut modifier.
    A,
};

/// Holds the input state for one devgui frame.
///
/// Positions are in the pixels of devgui's styles, with the origin at the
/// top-left corner of the viewport and the y axis pointing down. Without a UI
/// scale these are the logical pixels that `Mouse::getPosition()` reports.
/// Button states are snapshots; the context derives presses and releases by
/// comparing them with the previous frame.
struct Input final {
    /// The size of the viewport.
    Vector2 viewportSize = Vector2{0.0f, 0.0f};

    /// The pointer position.
    Vector2 pointerPosition = Vector2{0.0f, 0.0f};

    /// The scroll wheel movement during this frame, in notches.
    ///
    /// Positive y means the wheel moved away from the user, which scrolls the
    /// content toward its top.
    Vector2 wheelDelta = Vector2{0.0f, 0.0f};

    /// The time in seconds since an arbitrary monotonic origin.
    ///
    /// Widgets use it only for deadlines requested with
    /// `UI::requestUpdateAt()`.
    f64 timeSeconds = 0.0;

    /// True when the pointer is inside the viewport.
    bool pointerPresent = false;

    /// True while the primary pointer button is held.
    bool primaryButtonDown = false;

    /// True while the secondary pointer button, usually the right mouse
    /// button, is held. Releasing it over a widget opens its context menu.
    bool secondaryButtonDown = false;

    /// The UTF-8 text typed during this frame, which the focused text field
    /// inserts.
    ///
    /// Control characters are ignored; editing keys arrive through
    /// editKeysDown instead. The view must stay valid until the frame ends.
    /// Applications with a text input service leave it empty and pass
    /// textInputEvents instead.
    std::string_view textInput = {};

    /// The events of a text input service during this frame: committed text
    /// and the text that an input method editor is composing.
    ///
    /// A text field applies the events whose field ID is its widget ID, the
    /// value that `Context::getFocusedTextField()` reports. The span must stay
    /// valid until the frame ends.
    std::span<const textinput::TextInputEvent> textInputEvents = {};

    /// The editing keys held down, one bit per `EditKey`. Use `setKeyDown()`
    /// and `isKeyDown()` rather than the bits.
    u32 editKeysDown = 0;

    /// True while a shift key is held. Drag widgets move in finer steps, and
    /// arrow keys extend the text selection.
    bool shiftDown = false;

    /// True while the platform's shortcut modifier is held: Command on macOS
    /// and Control elsewhere.
    bool shortcutDown = false;

    /// Marks key as held down or released.
    void
    setKeyDown(EditKey key, bool down) noexcept
    {
        const auto bit = u32(1) << static_cast<u32>(key);
        if (down) {
            editKeysDown |= bit;
        }
        else {
            editKeysDown &= ~bit;
        }
    }

    /// Returns true while key is held down.
    [[nodiscard]] bool
    isKeyDown(EditKey key) const noexcept
    {
        return (editKeysDown & (u32(1) << static_cast<u32>(key))) != 0;
    }
};

/// Returns the input state read from mouse, keyboard, and textInput.
///
/// This is an optional helper for the common setup where devgui covers the
/// whole window. keyboard and textInput may be null. Typed text comes from
/// textInput's events when it is given, and from the keyboard otherwise.
/// Applications with other input sources fill `Input` directly.
///
/// viewportSize is the window's drawable area in logical pixels. Pass the back
/// buffer size divided by the pixel ratio rather than the client size, which
/// the platform rounds to whole logical pixels, so that each logical pixel
/// maps to the same physical pixels in every frame.
///
/// uiScale enlarges the whole UI on top of the platform's pixel ratio, for
/// example 1.5 for a 150% zoom: the viewport and the pointer position are
/// divided by it. Pass the pixel ratio multiplied by uiScale to
/// `Renderer::draw()` for the frame.
[[nodiscard]] Input
makeInput(
    const Mouse& mouse,
    raw_ptr<const Keyboard> keyboard,
    raw_ptr<const textinput::TextInputService> textInput,
    const Vector2& viewportSize,
    f64 timeSeconds,
    f32 uiScale = 1.0f) noexcept;

} // namespace pomdog::devgui
