// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/color.h"
#include "pomdog/math/vector2.h"

namespace pomdog::devgui {

/// Holds the sizes and colors that devgui widgets use.
///
/// Sizes are in logical pixels. Scale every size by the same factor to enlarge
/// the whole UI. The default colors follow the dark themes of current desktop
/// tools such as VS Code: flat surfaces, one-pixel borders, and a single blue
/// accent for focus and selection.
struct Style final {
    /// The height of a line of text from the font's ascender to its descender.
    ///
    /// This is the same unit as the fontSize passed to `createSpriteFont()`.
    /// For Noto Sans, 16 draws text about as large as 12-pixel CSS text.
    f32 fontSize = 16.0f;

    /// The distance between the tops of two lines of wrapped text, as a
    /// multiple of fontSize.
    f32 lineHeightScale = 1.25f;

    /// The space between a window border and its content.
    Vector2 windowPadding = Vector2{10.0f, 8.0f};

    /// The space between a framed widget's border and its text.
    Vector2 framePadding = Vector2{8.0f, 4.0f};

    /// The space between neighboring widgets.
    Vector2 itemSpacing = Vector2{8.0f, 4.0f};

    /// The space around the content of popups and tooltips.
    Vector2 popupPadding = Vector2{8.0f, 6.0f};

    /// The horizontal offset of the children of an open tree node.
    f32 indent = 16.0f;

    /// The height of a window's title bar.
    f32 titleBarHeight = 30.0f;

    /// The corner radius of framed widgets such as buttons and text fields.
    f32 frameRounding = 3.0f;

    /// The corner radius of windows.
    f32 windowRounding = 6.0f;

    /// The corner radius of popups and tooltips.
    f32 popupRounding = 5.0f;

    /// The width of a scroll bar.
    f32 scrollbarWidth = 10.0f;

    /// The distance a region scrolls for one wheel notch.
    f32 scrollStep = 48.0f;

    /// The fraction of a row's width that the label column of a property widget
    /// such as a slider takes.
    f32 labelColumnRatio = 0.4f;

    /// The size of the resize grip at the bottom-right corner of a window.
    f32 resizeGripSize = 14.0f;

    /// The smallest size a window can be resized to.
    Vector2 minWindowSize = Vector2{160.0f, 80.0f};

    /// The time in seconds the pointer rests on a widget before its tooltip
    /// appears.
    f64 tooltipDelay = 0.5;

    /// The width at which tooltip text wraps.
    f32 tooltipMaxWidth = 320.0f;

    Color textColor = Color{204, 204, 204, 255};
    Color textSecondaryColor = Color{157, 157, 157, 255};
    Color textOnAccentColor = Color{255, 255, 255, 255};

    Color windowBackgroundColor = Color{31, 31, 31, 250};
    Color titleBarColor = Color{24, 24, 24, 255};
    Color borderColor = Color{43, 43, 43, 255};
    Color shadowColor = Color{0, 0, 0, 72};

    Color inputBackgroundColor = Color{49, 49, 49, 255};
    Color inputBorderColor = Color{60, 60, 60, 255};
    Color inputHoveredBorderColor = Color{85, 85, 85, 255};

    /// The color of focus rings, selected items, and filled parts of controls.
    Color accentColor = Color{0, 120, 212, 255};
    Color accentHoveredColor = Color{2, 110, 193, 255};
    Color accentActiveColor = Color{0, 95, 170, 255};

    Color sliderTrackColor = Color{60, 60, 60, 255};
    Color sliderThumbColor = Color{204, 204, 204, 255};
    Color sliderThumbHoveredColor = Color{255, 255, 255, 255};

    Color rowHoveredColor = Color{42, 45, 46, 255};
    Color rowSelectedColor = Color{4, 57, 94, 255};
    Color separatorColor = Color{43, 43, 43, 255};

    Color scrollbarThumbColor = Color{121, 121, 121, 102};
    Color scrollbarThumbHoveredColor = Color{100, 100, 100, 179};
    Color scrollbarThumbActiveColor = Color{191, 191, 191, 102};

    Color popupBackgroundColor = Color{37, 37, 38, 255};
    Color popupBorderColor = Color{69, 69, 69, 255};
    Color tooltipBackgroundColor = Color{32, 32, 32, 255};

    /// The color of the text selection in a text field.
    Color selectionColor = Color{38, 79, 120, 255};

    /// The color of the line that shows where a dragged tab or row will go.
    Color dropIndicatorColor = Color{204, 204, 204, 255};

    /// The outline drawn around widgets whose ID is used twice in one frame.
    Color errorColor = Color{241, 76, 76, 255};

    /// Returns the height of a single-line framed widget such as a button.
    [[nodiscard]] f32
    getFrameHeight() const noexcept
    {
        return fontSize + framePadding.y * 2.0f;
    }

    /// Returns the distance between the tops of two lines of wrapped text.
    [[nodiscard]] f32
    getLineHeight() const noexcept
    {
        return fontSize * lineHeightScale;
    }
};

} // namespace pomdog::devgui
