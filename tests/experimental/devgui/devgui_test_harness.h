// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/experimental/devgui/devgui.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

// NOTE: devgui tests use a fixed style, so that tuning the default style does
// not move the positions they check. Its frame height is 20, its window padding
// is (8, 6), and its item spacing is (6, 4). Widgets outside any window start at
// (8, 6), and widgets in a window start at (window.x + 8, window.y + 26) below
// the 20-pixel title bar. Text is 7 pixels wide per byte.
//
// A property row outside any window spans x = 8 to 792. The label column takes
// floor(784 * 0.4) = 313 pixels, so the control spans x = 321 to 792.

namespace devgui_test {

using pomdog::f32;
using pomdog::f64;
using pomdog::Vector2;

POMDOG_CLANG_SUPPRESS_WARNING_PUSH
// NOTE: This header-only test helper has no source file to hold its vtable, so
// each test translation unit emits a copy, which is harmless here.
POMDOG_CLANG_SUPPRESS_WARNING("-Wweak-vtables")

/// Measures every byte as half the font size wide.
class FixedWidthMeasurer final : public pomdog::devgui::TextMeasurer {
public:
    [[nodiscard]] f32
    measureWidth(std::string_view text, f32 fontSize) override
    {
        return static_cast<f32>(text.size()) * fontSize * 0.5f;
    }
};

POMDOG_CLANG_SUPPRESS_WARNING_POP

[[nodiscard]] inline pomdog::devgui::Style
makeTestStyle()
{
    pomdog::devgui::Style style;
    style.fontSize = 14.0f;
    style.windowPadding = Vector2{8.0f, 6.0f};
    style.framePadding = Vector2{6.0f, 3.0f};
    style.itemSpacing = Vector2{6.0f, 4.0f};
    style.popupPadding = Vector2{8.0f, 6.0f};
    style.indent = 14.0f;
    style.titleBarHeight = 20.0f;
    style.scrollStep = 40.0f;
    style.labelColumnRatio = 0.4f;
    style.tooltipDelay = 0.5;
    style.tooltipMaxWidth = 320.0f;
    return style;
}

/// Builds frames on a context with input that tests change between frames.
class Harness final {
public:
    pomdog::devgui::Context context{std::make_shared<FixedWidthMeasurer>()};
    pomdog::devgui::Input input;

    Harness()
    {
        context.setStyle(makeTestStyle());
        input.viewportSize = Vector2{800.0f, 600.0f};
        input.pointerPresent = true;
        input.pointerPosition = Vector2{500.0f, 500.0f};
    }

    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;
    Harness(Harness&&) = delete;
    Harness& operator=(Harness&&) = delete;
    ~Harness() = default;

    /// Builds one frame. Typed text lasts for that frame only.
    template <typename Func>
    void frame(Func&& build)
    {
        {
            pomdog::devgui::UI ui{context, input};
            build(ui);
        }
        input.textInput = {};
    }

    void moveTo(f32 x, f32 y)
    {
        input.pointerPosition = Vector2{x, y};
    }

    /// Presses and releases the pointer at (x, y) over three frames.
    template <typename Func>
    void click(f32 x, f32 y, Func&& build)
    {
        moveTo(x, y);
        frame(build);
        input.primaryButtonDown = true;
        frame(build);
        input.primaryButtonDown = false;
        frame(build);
    }

    /// Presses and releases key over two frames.
    template <typename Func>
    void pressKey(pomdog::devgui::EditKey key, Func&& build)
    {
        input.setKeyDown(key, true);
        frame(build);
        input.setKeyDown(key, false);
        frame(build);
    }
};

[[nodiscard]] inline pomdog::devgui::WindowOptions
windowAt(f32 x, f32 y, f32 width, f32 height)
{
    return pomdog::devgui::WindowOptions{
        .position = Vector2{x, y},
        .size = Vector2{width, height},
        .collapsed = false,
    };
}

/// Returns true when the draw list draws text exactly equal to text.
///
/// NOTE: text is a C string because MSVC warns (C4866) when a REQUIRE argument
/// converts a string literal to a std::string_view.
[[nodiscard]] inline bool
drawsText(const pomdog::devgui::DrawList& drawList, const char* text)
{
    for (const auto& command : drawList.getCommands()) {
        if ((command.kind == pomdog::devgui::DrawCommandKind::Text) && (drawList.getText(command) == std::string_view{text})) {
            return true;
        }
    }
    return false;
}

} // namespace devgui_test
