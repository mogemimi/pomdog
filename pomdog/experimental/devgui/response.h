// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/devgui/key.h"
#include "pomdog/math/f32_rect2d.h"

namespace pomdog::devgui {

/// Reports how the user interacted with a widget during one frame.
///
/// A widget becomes active when the pointer is pressed on it and stays active
/// until the pointer is released, even if the pointer leaves the widget.
/// For value widgets such as sliders, `activated` and `deactivated` mark the
/// start and end of one edit, which is where an editor opens and closes an undo
/// step. `changed` is set only on frames where the user changed the value.
struct Response final {
    /// The widget's ID.
    WidgetID id = {};

    /// The area the widget takes, in logical pixels.
    F32Rect2D rect = {0.0f, 0.0f, 0.0f, 0.0f};

    /// True when the pointer is over the widget and no other widget is active.
    bool hovered = false;

    /// True while the widget is active.
    bool active = false;

    /// True on the frame the widget became active.
    bool activated = false;

    /// True on the frame the widget stopped being active.
    bool deactivated = false;

    /// True when the pointer was released over the widget that it was pressed on.
    bool clicked = false;

    /// True on the frame the widget became active by the second of two quick
    /// presses at the same place.
    bool doubleClicked = false;

    /// True when the secondary button, usually the right mouse button, was
    /// released over the widget while no other widget was active. Context
    /// menus open on it.
    bool secondaryClicked = false;

    /// True when the user changed the widget's value this frame.
    bool changed = false;

    /// Returns true when the widget held the pointer during this frame,
    /// including the frame of the release. Dragging widgets read the pointer
    /// on these frames so that the last movement before the release counts.
    [[nodiscard]] constexpr bool
    isHeld() const noexcept
    {
        return active || deactivated;
    }
};

} // namespace pomdog::devgui
