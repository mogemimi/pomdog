// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/devgui/input.h"
#include "pomdog/experimental/devgui/key.h"
#include "pomdog/experimental/devgui/response.h"
#include "pomdog/math/color.h"
#include "pomdog/math/f32_rect2d.h"
#include "pomdog/math/vector2.h"
#include "pomdog/memory/raw_ptr.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <optional>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
class Context;
struct DockSpaceOptions;
struct DrawCommand;
struct RectGradient;
struct Style;
struct TextEditState;
} // namespace pomdog::devgui

namespace pomdog::i18n {
enum class LineBreakLanguage : u8;
} // namespace pomdog::i18n

namespace pomdog::devgui {

/// Reports the state of a scroll region when it ends.
struct ScrollResult final {
    /// The scroll offset to keep for the next frame.
    f32 scrollY = 0.0f;

    /// The height of the region's content, including padding.
    f32 contentHeight = 0.0f;

    /// The width the region's items asked for, including padding and the
    /// scroll bar. Items of a fixed width ask for it, and items that fill the
    /// row ask with `UI::requestWidth()`.
    f32 contentWidth = 0.0f;
};

/// Selects where a popup appears next to the area it opens from.
enum class PopupPlacement : u8 {
    /// Below the area, or above it when there is more room there. Combo boxes
    /// and context menus open this way.
    Below,

    /// To the right of the area, or to its left when there is no room. Submenus
    /// open this way.
    Right,
};

/// Selects what a press outside every open popup does besides closing them.
enum class PopupOutsidePress : u8 {
    /// The press does nothing else, so clicking outside a menu only closes
    /// the menu.
    Consume,

    /// The press also reaches the widget under the pointer, so that a text
    /// field keeps working while its suggestions show below it.
    PassThrough,
};

/// Builds one devgui frame.
///
/// Constructing a `UI` starts a frame and destroying it finishes the frame:
/// ```cpp
/// if (context.needsUpdate(input)) {
///     devgui::UI ui{context, input};
///     devgui::text(ui, "Hello");
/// }
/// ```
/// Widgets are free functions that take `UI&`, such as `devgui::button()`.
/// The members below are the primitives those functions are built from: ID
/// scopes, layout, interaction, keyboard focus, popups, drawing, and per-widget
/// state. Application code uses the same members to write its own widgets.
///
/// A frame is single pass. Each widget reads input and draws when it is called,
/// so its response is final when the function returns. Only one `UI` can exist
/// per context at a time.
class UI final {
public:
    /// Starts a frame on context with input.
    UI(Context& context, const Input& input);

    /// Finishes the frame and keeps its draw list in the context.
    ~UI();

    UI(const UI&) = delete;
    UI& operator=(const UI&) = delete;
    UI(UI&&) = delete;
    UI& operator=(UI&&) = delete;

    /// Returns the ID of the widget that key names in the current ID scope.
    [[nodiscard]] WidgetID
    makeID(Key key) const noexcept;

    /// Makes key the innermost ID scope. Prefer `devgui::pushID()`, which pops
    /// the scope automatically.
    void
    pushIDScope(Key key);

    /// Removes the innermost ID scope.
    void
    popIDScope() noexcept;

    /// Reserves the next area in the current layout and returns it.
    ///
    /// A width of zero or less takes the rest of the row.
    [[nodiscard]] F32Rect2D
    allocate(f32 width, f32 height);

    /// Returns the width left for the next item in the current layout.
    [[nodiscard]] f32
    getAvailableWidth() const noexcept;

    /// Returns true when the current layout places items side by side.
    [[nodiscard]] bool
    isLayoutHorizontal() const noexcept;

    /// Places the following items side by side. Prefer `devgui::horizontal()`.
    void
    beginHorizontal();

    /// Ends the row started with `beginHorizontal()`.
    void
    endHorizontal();

    /// Places the following items one below another with spacing pixels
    /// between them, instead of the style's item spacing, for example to put
    /// the rows of a list right next to each other. The items take the place
    /// of one item in the current layout.
    void
    beginVertical(f32 spacing);

    /// Ends the column started with `beginVertical()`.
    void
    endVertical();

    /// Moves the left edge of the current layout by amount. Pass a negative
    /// amount to undo it.
    void
    indent(f32 amount) noexcept;

    /// Asks the enclosing region to show width pixels of the item allocated
    /// last, measured from the item's left edge.
    ///
    /// Items of a fixed width ask for it when they are allocated. Items that
    /// fill the row, such as menu items, call this with the width of their
    /// content, so that a popup sized to fit its content fits them.
    void
    requestWidth(f32 width) noexcept;

    /// Resolves hover and press for the widget id occupying rect.
    ///
    /// Call it once per interactive widget per frame. A widget is hovered only
    /// when the pointer is over rect inside the current clip rectangle and its
    /// window or popup is the topmost one under the pointer.
    [[nodiscard]] Response
    interact(WidgetID id, const F32Rect2D& rect);

    /// Resolves hover and press like `interact()` for a widget that lies
    /// behind widgets submitted after it in the same area, such as a list row
    /// with a close button on it.
    ///
    /// The widget stays hovered while the pointer is over a widget in front,
    /// but leaves the press to that widget.
    [[nodiscard]] Response
    interactBehind(WidgetID id, const F32Rect2D& rect);

    /// Returns true on the frame the primary pointer button went down.
    [[nodiscard]] bool
    isPointerPressed() const noexcept;

    /// Returns how far the pointer moved since the previous frame.
    [[nodiscard]] Vector2
    getPointerDelta() const noexcept;

    /// Returns where the primary pointer button last went down, for example
    /// to keep the point where the user grabbed a dragged item.
    [[nodiscard]] Vector2
    getPressPosition() const noexcept;

    /// Returns how long, in seconds, the pointer has rested on id, or zero
    /// when id is not hovered. Call it after `interact()` for id.
    [[nodiscard]] f64
    getHoveredDuration(WidgetID id) const noexcept;

    /// Returns a value the active widget can keep while it stays active, such
    /// as the fraction of a step left over from dragging. It is reset to zero
    /// when a widget becomes active.
    [[nodiscard]] f32&
    getActiveScratch() noexcept;

    /// Returns true when key went down this frame, or repeats because it is
    /// held while a widget has keyboard focus.
    [[nodiscard]] bool
    isKeyPressed(EditKey key) const noexcept;

    /// Gives keyboard focus to id. The widget keeps it until it calls
    /// `clearKeyboardFocus()` or stops calling `interact()`.
    void
    setKeyboardFocus(WidgetID id) noexcept;

    /// Removes keyboard focus from whichever widget has it.
    void
    clearKeyboardFocus() noexcept;

    /// Returns true when id has keyboard focus.
    [[nodiscard]] bool
    hasKeyboardFocus(WidgetID id) const noexcept;

    /// Returns the editing state shared by text fields. Only the widget with
    /// keyboard focus should change it.
    [[nodiscard]] TextEditState&
    getTextEditState() noexcept;

    /// Returns the value stored for id, or nullopt when none is stored.
    [[nodiscard]] std::optional<u64>
    loadState(WidgetID id) noexcept;

    /// Stores a value for id that persists across frames.
    void
    storeState(WidgetID id, u64 value) noexcept;

    /// Fills rect with color, rounding its corners by radius.
    void
    fillRect(const F32Rect2D& rect, const Color& color, f32 radius = 0.0f);

    /// Outlines rect with color, thickness pixels wide inside rect, rounding its
    /// corners by radius.
    void
    strokeRect(const F32Rect2D& rect, const Color& color, f32 thickness, f32 radius = 0.0f);

    /// Fills rect with a gradient between the corner colors of gradient.
    void
    fillRectGradient(const F32Rect2D& rect, const RectGradient& gradient);

    /// Fills the circle at center with color.
    void
    fillCircle(const Vector2& center, f32 radius, const Color& color);

    /// Fills the triangle p0, p1, p2 with color.
    void
    fillTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& color);

    /// Draws a line from start to end, thickness pixels wide.
    void
    drawLine(const Vector2& start, const Vector2& end, const Color& color, f32 thickness);

    /// Draws connected line segments from p0 through p1 to p2, thickness pixels wide.
    void
    drawPolyline(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& color, f32 thickness);

    /// Draws a single line of UTF-8 text with its top-left corner at position.
    ///
    /// The text is copied into the draw list, so it may live in a temporary
    /// buffer.
    void
    drawText(const Vector2& position, std::string_view text, const Color& color);

    /// Draws text like the overload above, but ends it with an ellipsis where
    /// it would be wider than maxWidth.
    void
    drawText(const Vector2& position, std::string_view text, const Color& color, f32 maxWidth);

    /// Returns the width of text drawn at the style's font size.
    [[nodiscard]] f32
    measureText(std::string_view text);

    /// Returns text with line breaks inserted so that each line fits within
    /// maxWidth, following the line breaking rules of language.
    ///
    /// The result is cached for id, so a widget that shows the same text every
    /// frame wraps it once. The returned view is valid until the frame ends.
    [[nodiscard]] std::string_view
    wrapText(WidgetID id, std::string_view text, f32 maxWidth, i18n::LineBreakLanguage language);

    /// Restricts drawing and input to rect intersected with the current clip
    /// rectangle.
    void
    pushClipRect(const F32Rect2D& rect);

    /// Restores the clip rectangle that was current before `pushClipRect()`.
    void
    popClipRect() noexcept;

    /// Returns true when part of rect is inside the current clip rectangle.
    ///
    /// Widgets far outside a scrolled view can skip their drawing work.
    [[nodiscard]] bool
    isVisible(const F32Rect2D& rect) const noexcept;

    /// Starts a window and returns true when its content is visible.
    ///
    /// key names the window across the whole UI, whatever the current ID
    /// scope, so that dock layouts and saved layouts can refer to it. A window
    /// docked in a dock space shows its content there, below its tab, and
    /// returns false while another tab is selected. initialRect and
    /// initiallyCollapsed apply only when the window first appears. Call
    /// `endWindow()` whether or not this returns true. Prefer
    /// `devgui::window()`.
    ///
    /// When open is not null, the window shows a close button on its title bar
    /// and on its tab, and clicking it sets *open to false. The window still
    /// finishes the frame; stop submitting it while *open is false.
    [[nodiscard]] bool
    beginWindow(Key key, std::string_view title, const F32Rect2D& initialRect, bool initiallyCollapsed, raw_ptr<bool> open);

    /// Ends the window started with `beginWindow()`.
    void
    endWindow();

    /// Lays out the dock space key in rect and handles its splits and tabs.
    /// Prefer `devgui::dockSpace()`.
    void
    submitDockSpace(Key key, const F32Rect2D& rect, const DockSpaceOptions& options);

    /// Opens the popup id.
    ///
    /// Popups form a stack, such as a context menu and its submenus. Opening a
    /// popup while building another one opens it as that popup's child,
    /// closing the child that was open before; opening one elsewhere closes
    /// every popup first. A popup closes when the user presses outside it or
    /// its children, when Escape closes the innermost popup, when
    /// `closePopup()` is called, or when a frame does not submit it with
    /// `beginPopup()`. outsidePress decides whether a press that closes the
    /// popups does anything else.
    void
    openPopup(WidgetID id, PopupOutsidePress outsidePress = PopupOutsidePress::Consume);

    /// Closes every open popup, for example after the user chose a menu item.
    void
    closePopup() noexcept;

    /// Closes the children of the popup being built, such as a submenu that
    /// another item of the same menu should replace.
    void
    closeChildPopups() noexcept;

    /// Returns true when the popup id is open.
    [[nodiscard]] bool
    isPopupOpen(WidgetID id) const noexcept;

    /// Returns true when the pointer is over the popup id and no popup lies
    /// above it there, judged from the popups of the previous frame.
    [[nodiscard]] bool
    isPointerOverPopup(WidgetID id) const noexcept;

    /// Returns true when the popup being built has an open child, such as a
    /// submenu. Outside popups, returns true when any popup is open.
    [[nodiscard]] bool
    hasChildPopups() const noexcept;

    /// Returns where the pointer was when the popup id opened, or where it is
    /// now when the popup is not open.
    [[nodiscard]] Vector2
    getPopupOpenPosition(WidgetID id) const noexcept;

    /// Starts drawing the open popup id above every window.
    ///
    /// placement decides where the popup appears next to anchor. It keeps
    /// inside the viewport and takes size, or as much of it as fits. A size
    /// component of zero or less fits the content measured in the previous
    /// frame; on the first frame the popup measures its content without showing
    /// it. Content taller than the popup scrolls. Call it only while
    /// `isPopupOpen(id)` is true, and end it with `endPopup()`.
    void
    beginPopup(WidgetID id, const F32Rect2D& anchor, const Vector2& size, PopupPlacement placement = PopupPlacement::Below);

    /// Ends the popup started with `beginPopup()`.
    void
    endPopup();

    /// Starts drawing above every window and popup without taking input, for
    /// example for a tooltip. Drawing is clipped only by the viewport. End it
    /// with `endOverlay()`.
    void
    beginOverlay();

    /// Ends the overlay started with `beginOverlay()`.
    void
    endOverlay();

    /// Starts a vertically scrolling region that shows viewRect.
    ///
    /// scrollY and lastContentHeight are the values the matching
    /// `endScrollRegion()` returned in the previous frame. The caller stores
    /// them, typically with `storeState()`.
    void
    beginScrollRegion(WidgetID id, const F32Rect2D& viewRect, f32 scrollY, f32 lastContentHeight, const Vector2& padding);

    /// Ends the innermost scroll region and returns the values to keep.
    [[nodiscard]] ScrollResult
    endScrollRegion();

    /// Scrolls the innermost scroll region so that rect, which a widget took
    /// in this frame, is in view from the next frame, for example when the
    /// keyboard moves a highlight onto a row.
    void
    scrollIntoView(const F32Rect2D& rect);

    /// Asks for a new frame once input time reaches timeSeconds, for example
    /// to blink a caret or show a delayed tooltip.
    void
    requestUpdateAt(f64 timeSeconds) noexcept;

    [[nodiscard]] const Input&
    getInput() const noexcept;

    [[nodiscard]] const Style&
    getStyle() const noexcept;

private:
    /// Holds the layer that a popup or an overlay temporarily replaces.
    struct LayerState final {
        WidgetID window = {};
        i32 windowIndex = -1;
        u64 depth = 0;
    };

    /// Holds one popup being built.
    struct PopupFrame final {
        LayerState saved = {};
        WidgetID id = {};
        F32Rect2D rect = {0.0f, 0.0f, 0.0f, 0.0f};

        /// The content size measured in an earlier frame, if any.
        std::optional<Vector2> measuredSize;

        /// True when the popup lays out its content without showing it.
        bool measuring = false;
    };

    /// Selects how the current window is shown.
    enum class WindowMode : u8 {
        Floating,
        Docked,

        /// Docked behind another tab, or in a dock space this frame did not
        /// submit.
        Hidden,
    };

    /// The deepest popup nesting, such as a menu with submenus.
    static constexpr std::size_t maxPopupDepth = 8;

    [[nodiscard]] const F32Rect2D&
    getClipRect() const noexcept;

    [[nodiscard]] Response
    resolveInteraction(WidgetID id, const F32Rect2D& rect, bool behind);

    [[nodiscard]] bool
    isPointerOverCurrentLayer() const noexcept;

    [[nodiscard]] LayerState
    enterLayer(WidgetID layer, u64 depth);

    void
    leaveLayer(const LayerState& saved);

    void
    updateKeyRepeat();

    void
    updatePopupsAtFrameStart();

    void
    updatePopupsAtFrameEnd();

    /// Returns the dock space, or the docked window, under point in the
    /// layout of the previous frame, or zero when there is none.
    [[nodiscard]] WidgetID
    hitTestDockSpaces(const Vector2& point) const noexcept;

    /// Returns the dock space and leaf node where window is docked.
    [[nodiscard]] bool
    findDockedNode(WidgetID window, std::size_t& dockSpaceIndex, i32& nodeIndex) const noexcept;

    /// Makes the window float with grabOffset from its top-left corner under
    /// the pointer, and lets the pointer keep dragging it by the title bar.
    void
    startWindowDrag(std::size_t windowIndex, const Vector2& grabOffset);

    /// Finishes dragging a window over dock spaces: shows where it would dock
    /// and docks it when released there.
    void
    updateWindowDocking();

    void
    addCommand(const DrawCommand& command, std::string_view text = {});

private:
    Context& context_;
    Input input_;
    Vector2 pointerDelta_ = Vector2{0.0f, 0.0f};
    std::array<PopupFrame, maxPopupDepth> popupFrames_ = {};
    LayerState overlaySaved_ = {};
    WidgetID hoveredWindow_ = {};
    WidgetID currentWindow_ = {};
    WidgetID hoveredID_ = {};
    WidgetID wheelCandidate_ = {};
    F32Rect2D hoveredRect_ = {0.0f, 0.0f, 0.0f, 0.0f};

    /// The floating window the pointer drags far enough to dock it.
    WidgetID draggedWindow_ = {};
    std::optional<f64> wakeTime_;
    f64 hoveredSince_ = 0.0;
    u64 currentDepth_ = 0;
    u32 keysPressed_ = 0;
    i32 currentWindowIndex_ = -1;

    /// The level of the popup being built, or -1 outside popups.
    i32 popupLevel_ = -1;
    WindowMode windowMode_ = WindowMode::Floating;
    bool pressed_ = false;
    bool released_ = false;
    bool secondaryPressed_ = false;
    bool secondaryReleased_ = false;
    bool pressConsumed_ = false;
    bool wheelConsumed_ = false;
    bool activeSeen_ = false;
    bool focusSeen_ = false;
    bool inOverlay_ = false;
    bool windowContentOpen_ = false;
    bool gripHovered_ = false;
};

} // namespace pomdog::devgui
