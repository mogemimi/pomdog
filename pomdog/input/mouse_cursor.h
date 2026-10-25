// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog {

/// Identifies a mouse cursor shape that the platform provides.
///
/// `GameWindow::requestMouseCursor()` shows it over the window. Platforms
/// without a mouse cursor, or without some of these shapes, report it with
/// `GameWindow::supportsMouseCursor()`.
enum class MouseCursor : u8 {
    /// The standard arrow cursor.
    Arrow,

    /// The I-beam cursor, used when hovering over selectable text.
    IBeam,

    /// The pointing hand cursor, used for clickable elements such as links.
    PointingHand,

    /// The vertical resize cursor, used when resizing along the vertical axis.
    ResizeVertical,

    /// The horizontal resize cursor, used when resizing along the horizontal axis.
    ResizeHorizontal,
};

} // namespace pomdog
