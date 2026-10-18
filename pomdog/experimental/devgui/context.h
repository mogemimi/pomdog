// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/devgui/dock_space.h"
#include "pomdog/experimental/devgui/draw_list.h"
#include "pomdog/experimental/devgui/input.h"
#include "pomdog/experimental/devgui/key.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/text_edit_state.h"
#include "pomdog/experimental/devgui/widget_state_storage.h"
#include "pomdog/experimental/i18n/line_break_language.h"
#include "pomdog/experimental/text_input/text_field_state.h"
#include "pomdog/math/f32_rect2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <deque>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::clipboard {
class ClipboardService;
} // namespace pomdog::clipboard

namespace pomdog::devgui {
class TextMeasurer;
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui::detail {

/// The root of every ID scope. Any nonzero constant works.
///
/// Windows and dock spaces take their IDs directly under the root, whatever
/// the current ID scope, so that layouts can name them by key alone.
inline constexpr WidgetID rootID = WidgetID{0x9E3779B97F4A7C15ULL};

struct WindowState final {
    WidgetID id = {};

    /// The key the window was created with, which layouts refer to it by.
    Key key = {};

    /// The area of the window while it floats.
    F32Rect2D rect = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 scrollY = 0.0f;
    f32 contentHeight = 0.0f;

    /// Orders windows back to front. A larger depth is drawn in front.
    u64 depth = 0;

    /// The number of the last frame that submitted the window.
    u64 lastFrame = 0;

    /// The title from the last frame, which dock tab bars show before the
    /// window is submitted again.
    std::string title;

    /// False for a window that a dock layout named before it first appeared.
    /// The window takes its initial area when it does.
    bool hasRect = true;

    bool collapsed = false;

    /// True when the window was docked in the last frame that submitted it.
    bool docked = false;

    /// True when the last frame that submitted the window let the user close
    /// it, which shows a close button on its tab.
    bool closable = false;

    /// True after the user clicked the close button on the window's tab, until
    /// the window is submitted again in the same frame.
    bool closeRequested = false;
};

struct LayoutFrame final {
    f32 left = 0.0f;
    f32 right = 0.0f;
    f32 top = 0.0f;
    f32 cursorX = 0.0f;
    f32 cursorY = 0.0f;
    f32 maxY = 0.0f;
    f32 rowHeight = 0.0f;

    /// The left edge of the item allocated last.
    f32 lastItemX = 0.0f;

    /// The rightmost edge that items asked to show, which measures the width
    /// of the content.
    f32 requestedRight = 0.0f;

    /// The space between neighboring items.
    Vector2 spacing = Vector2{0.0f, 0.0f};
    bool horizontal = false;
    bool hasItems = false;
};

/// Holds one level of the stack of open popups.
struct PopupState final {
    WidgetID id = {};

    /// The area of the popup in the last frame that submitted it.
    F32Rect2D rect = {0.0f, 0.0f, 0.0f, 0.0f};

    /// The pointer position when the popup opened. Context menus open there.
    Vector2 openPosition = Vector2{0.0f, 0.0f};
    f32 scrollY = 0.0f;
    bool submitted = false;
    bool openedThisFrame = false;

    /// True when a press outside every popup only closes them, and false
    /// when the press also reaches the widget under the pointer.
    bool consumeOutsidePress = true;
};

/// Holds one node of a dock space's layout tree.
struct DockNode final {
    i32 parent = -1;

    /// The children of a split node, or -1 for a leaf node.
    std::array<i32, 2> children = {{-1, -1}};
    DockSplit split = DockSplit::LeftRight;

    /// The first child's share of the node's area.
    f32 ratio = 0.5f;

    /// The windows docked in a leaf node, in tab order.
    std::vector<WidgetID> tabs;
    WidgetID selectedTab = {};

    /// The area of the node in the current frame.
    F32Rect2D rect = {0.0f, 0.0f, 0.0f, 0.0f};

    /// The area of a leaf node below its tab bar, where the selected window
    /// shows its content.
    F32Rect2D contentRect = {0.0f, 0.0f, 0.0f, 0.0f};

    /// False for a slot that a removed node left free.
    bool used = true;

    [[nodiscard]] bool
    isLeaf() const noexcept
    {
        return children[0] < 0;
    }
};

struct DockSpaceState final {
    WidgetID id = {};
    Key key = {};

    /// The nodes of the layout tree. The root is always the first node.
    std::vector<DockNode> nodes;
    F32Rect2D rect = {0.0f, 0.0f, 0.0f, 0.0f};
    u64 lastFrame = 0;

    /// True when the last frame that submitted the dock space showed its tab
    /// bars.
    bool showTabBars = true;
};

/// Holds the place of one tab in a dock node's tab bar during a frame.
struct DockTabLayout final {
    /// The position of the tab in the node's tabs.
    std::size_t tabIndex = 0;
    std::size_t windowIndex = 0;
    F32Rect2D rect = {0.0f, 0.0f, 0.0f, 0.0f};
};

struct ScrollFrame final {
    WidgetID id = {};
    F32Rect2D viewRect = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 scrollY = 0.0f;
    f32 maxScroll = 0.0f;
    f32 lastContentHeight = 0.0f;
    f32 paddingX = 0.0f;
    f32 paddingY = 0.0f;

    /// The scroll offset that `UI::scrollIntoView()` asked for, if any.
    f32 requestedScrollY = 0.0f;
    bool hasRequestedScroll = false;
    bool hasScrollbar = false;
};

/// Caches the result of wrapping one widget's text.
struct WrappedTextEntry final {
    WidgetID id = {};
    std::string sourceText;
    std::string wrappedText;
    f32 maxWidth = 0.0f;
    f32 fontSize = 0.0f;
    u64 lastUsedFrame = 0;
    i18n::LineBreakLanguage language = i18n::LineBreakLanguage::Default;
};

/// Records the widget IDs used in one frame to detect duplicates.
class WidgetIDSet final {
public:
    /// Removes all IDs while keeping the allocated buckets.
    void
    clear() noexcept;

    /// Adds id and returns false if it was already present.
    [[nodiscard]] bool
    insert(WidgetID id);

private:
    void
    grow();

private:
    std::vector<u64> buckets_;
    u32 count_ = 0;
};

} // namespace pomdog::devgui::detail

namespace pomdog::devgui {

/// Reports which input devgui used, so that the game can ignore it.
struct InputCapture final {
    /// True when the pointer is over devgui or a devgui widget holds the press.
    bool pointer = false;

    /// True when devgui scrolled a region with the wheel.
    bool wheel = false;

    /// True when a devgui text field has keyboard focus.
    bool keyboard = false;
};

/// Holds the state of one devgui instance that persists across frames.
///
/// An application keeps one context per independent UI, such as an editor
/// overlay, and builds a frame by constructing a `UI` from it. The context
/// owns the draw list of the last frame, so a renderer can draw it again
/// without building a new frame.
///
/// Building a frame costs CPU time even when nothing changed. Call
/// `needsUpdate()` first and skip the frame when it returns false; the previous
/// draw list and input capture stay valid.
///
/// A context is not thread-safe. Use it from one thread.
class Context final {
public:
    /// Creates a context that measures text with textMeasurer.
    ///
    /// stateCapacity bounds the number of per-widget values, such as tree node
    /// states, kept across frames; see `WidgetStateStorage`. It must be between
    /// 1 and 262143, the entity limit of `ecs::EntityPool`.
    explicit Context(std::shared_ptr<TextMeasurer> textMeasurer, u32 stateCapacity = 1024);

    ~Context();

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

    /// Returns true when input may change the UI and a new frame should be built.
    ///
    /// Returns false when, for example, the pointer moves over the game outside
    /// every devgui window and no widget is active. A call to `invalidate()`,
    /// a viewport resize, or a deadline from `UI::requestUpdateAt()` also
    /// makes it return true.
    /// Button and editing-key transitions request a frame even outside the UI,
    /// so building it records their history before later input arrives. This
    /// query does not change the context; UI records input when a frame ends.
    [[nodiscard]] bool
    needsUpdate(const Input& input) const noexcept;

    /// Requests a new frame, for example after the application changed a value
    /// that the UI displays.
    void
    invalidate() noexcept;

    /// Returns the draw list of the last built frame.
    [[nodiscard]] const DrawList&
    getDrawList() const noexcept;

    /// Returns which input the last built frame used.
    [[nodiscard]] InputCapture
    getInputCapture() const noexcept;

    /// Sets the clipboard that text fields copy to and paste from, or removes
    /// it with nullptr. Without a clipboard, copy and paste do nothing.
    ///
    /// Call the service's `update()` before each frame; a frame applies the
    /// events of the last update. Build at most one frame per update, so that
    /// no paste applies twice.
    void
    setClipboardService(std::shared_ptr<clipboard::ClipboardService> clipboard) noexcept;

    /// Returns the text field that had keyboard focus when the last built
    /// frame ended, or nullopt when none had, for a text input service.
    ///
    /// Its id is the field's widget ID, which events for the field must carry
    /// in `Input::textInputEvents`. caretRect is in devgui's pixels; multiply it
    /// by the UI scale passed to `makeInput()` to get the window's logical
    /// pixels. surroundingText stays valid until the next frame starts.
    [[nodiscard]] std::optional<textinput::TextFieldState>
    getFocusedTextField() const noexcept;

    [[nodiscard]] const Style&
    getStyle() const noexcept;

    /// Replaces the style and requests a new frame.
    void
    setStyle(const Style& style) noexcept;

    /// Returns the number of frames built so far.
    [[nodiscard]] u64
    getFrameCount() const noexcept;

    /// Returns the number of widget IDs used more than once in the last frame.
    ///
    /// Widgets with a duplicate ID ignore input and are outlined with
    /// `Style::errorColor`. A correct UI keeps this at zero.
    [[nodiscard]] u32
    getDuplicateIDCount() const noexcept;

    /// Returns the window layout as text: where windows float, whether they
    /// are collapsed, and the splits and tabs of every dock space.
    ///
    /// Windows and dock spaces are written by their keys, so a later run of
    /// the same application can restore the layout with `loadLayout()`. The
    /// application decides where to keep the text.
    [[nodiscard]] std::string
    saveLayout() const;

    /// Replaces the window layout with one that `saveLayout()` wrote.
    ///
    /// Windows that the layout names but the application has not created yet
    /// take their place when they first appear. Returns an error and keeps the
    /// current layout when text is not a valid layout. Call it between frames.
    [[nodiscard]] std::unique_ptr<Error>
    loadLayout(std::string_view text);

    /// Returns true when the dock space key has a layout, from an earlier frame,
    /// a `DockLayoutBuilder`, or `loadLayout()`.
    [[nodiscard]] bool
    hasDockSpace(Key key) const noexcept;

    /// Replaces the contents of result with the windows docked in the dock
    /// space dockSpaceKey, node by node, and each node's windows in tab order.
    ///
    /// Only windows whose tabs show are listed. During a frame those are the
    /// windows submitted in that frame or the previous one; between frames,
    /// the windows the last frame submitted. An application builds its own
    /// list of tabs from it, such as a list view beside the dock space.
    void
    getDockedWindows(Key dockSpaceKey, std::vector<DockedWindow>& result) const;

    /// Shows the window windowKey: selects its tab when it is docked, or
    /// brings it to the front when it floats.
    void
    selectWindow(Key windowKey) noexcept;

    /// Moves the tab of windowKey to where the tab of targetKey is, shifting
    /// the tabs between them. Does nothing unless both windows are docked in
    /// the same node.
    void
    moveWindowTab(Key windowKey, Key targetKey) noexcept;

private:
    friend class UI;
    friend class DockLayoutBuilder;

    [[nodiscard]] bool
    isPointerOverUI(const Vector2& position) const noexcept;

    [[nodiscard]] F32Rect2D
    getWindowHitRect(const detail::WindowState& window) const noexcept;

private:
    std::shared_ptr<TextMeasurer> textMeasurer_;
    Style style_;
    DrawList drawList_;
    WidgetStateStorage states_;
    std::vector<detail::WindowState> windows_;

    // NOTE: Per-frame scratch buffers. They live here so that their capacity is
    // reused from frame to frame.
    std::vector<WidgetID> idStack_;
    std::vector<u16> clipStack_;
    std::vector<detail::LayoutFrame> layoutStack_;
    std::vector<detail::ScrollFrame> scrollStack_;
    std::vector<F32Rect2D> backgroundHotRects_;
    std::vector<detail::DockTabLayout> dockTabs_;
    detail::WidgetIDSet frameIDs_;

    // NOTE: deque keeps returned text views stable when the current frame
    // needs more slots. Entries not used this frame can be reused; capacity
    // grows only to accommodate distinct results used in the largest frame.
    std::deque<detail::WrappedTextEntry> wrappedTexts_;

    // NOTE: The open popups from the outermost to the innermost, such as a
    // context menu and its submenus.
    std::vector<detail::PopupState> popups_;

    std::vector<detail::DockSpaceState> dockSpaces_;

    TextEditState textEdit_;

    // NOTE: The text field that reported itself in the last frame, whose
    // surrounding text views focusedTextFieldText_.
    std::optional<textinput::TextFieldState> focusedTextField_;
    std::string focusedTextFieldText_;
    std::shared_ptr<clipboard::ClipboardService> clipboard_;
    Vector2 lastPressPosition_ = Vector2{0.0f, 0.0f};

    Input lastInput_ = {};
    WidgetID activeID_ = {};
    WidgetID hoveredID_ = {};
    WidgetID hoveredWindow_ = {};

    // NOTE: The area of hoveredID_, which tells a widget behind it whether the
    // pointer is still over the widget in front.
    F32Rect2D hoveredRect_ = {0.0f, 0.0f, 0.0f, 0.0f};
    WidgetID wheelTarget_ = {};
    WidgetID focusedID_ = {};
    WidgetID lastPressID_ = {};

    // NOTE: The widget the secondary button went down on. Releasing it over
    // the same widget opens its context menu.
    WidgetID secondaryPressID_ = {};
    std::optional<f64> wakeTime_;
    InputCapture capture_ = {};
    f64 hoveredSince_ = 0.0;
    f64 lastPressTime_ = 0.0;
    f64 repeatTime_ = 0.0;
    u64 frameCount_ = 0;
    u64 depthCounter_ = 0;
    f32 activeScratch_ = 0.0f;
    u32 duplicateIDCount_ = 0;
    u32 repeatKeys_ = 0;
    bool invalidated_ = true;
    bool frameActive_ = false;
};

} // namespace pomdog::devgui
