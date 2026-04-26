// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog {

/// Specifies the window mode.
///
/// Pomdog does not support exclusive fullscreen (also called "true fullscreen"
/// or "ExclusiveFullScreen") on any platform. When "fullscreen" is mentioned
/// without qualification, it refers to borderless fullscreen (a maximized
/// borderless window covering the entire screen).
enum class WindowMode : u8 {
    /// Standard windowed mode with a title bar and borders.
    Windowed,

    /// Borderless windowed mode without a title bar and borders.
    BorderlessWindowed,

    /// Soft fullscreen mode in a web browser (Emscripten only).
    ///
    /// Uses emscripten_enter_soft_fullscreen(), which stretches the canvas
    /// to fill the browser viewport without requesting the browser's native
    /// fullscreen API.
    BrowserSoftFullscreen,

    /// Maximized window mode (desktop platforms only).
    ///
    /// The window occupies all available desktop space while keeping
    /// the OS taskbar and window decorations visible.
    Maximized,

    /// Borderless fullscreen mode.
    ///
    /// A borderless window that covers the entire screen. This is the only
    /// fullscreen mode supported by Pomdog; exclusive fullscreen is not
    /// supported on Windows, Linux, macOS, or in the browser.
    Fullscreen,
};

} // namespace pomdog
