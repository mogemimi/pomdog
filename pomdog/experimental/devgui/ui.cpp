// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/context.h"
#include "pomdog/experimental/devgui/draw_list.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/text_edit_state.h"
#include "pomdog/experimental/devgui/text_measurer.h"
#include "pomdog/experimental/devgui/text_utility.h"
#include "pomdog/experimental/devgui/widget_helpers.h"
#include "pomdog/experimental/i18n/line_break_language.h"
#include "pomdog/experimental/i18n/text_wrap.h"
#include "pomdog/experimental/text_input/text_field_state.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <bit>
#include <cmath>
#include <iterator>
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
namespace {

using namespace pomdog::devgui::literals;

// NOTE: Overlays never take input, so their layer ID only has to differ from
// every window and popup ID.
constexpr WidgetID overlayLayerID = WidgetID{1};

constexpr Key windowTitleKey = "devgui.window.title"_key;
constexpr Key windowCollapseKey = "devgui.window.collapse"_key;
constexpr Key windowCloseKey = "devgui.window.close"_key;
constexpr Key windowGripKey = "devgui.window.grip"_key;
constexpr Key windowScrollKey = "devgui.window.scroll"_key;
constexpr Key popupScrollKey = "devgui.popup.scroll"_key;
constexpr Key scrollbarKey = "devgui.scrollbar"_key;

/// The depth of overlays such as tooltips, above every popup. Popups take the
/// depths just below it, one per nesting level.
constexpr u64 overlayDepth = std::numeric_limits<u64>::max();

/// The part of a window's title bar that must stay inside the viewport so the
/// user can drag the window back.
constexpr f32 windowGrabMargin = 40.0f;

/// The shortest scroll bar thumb.
constexpr f32 minThumbLength = 20.0f;

/// The space between a popup and the widget it opens from, and between a popup
/// and the viewport edges.
constexpr f32 popupMargin = 2.0f;

/// The narrowest popup that fits its content, so that rows that fill the
/// width without asking for it stay readable.
constexpr f32 minFitPopupWidth = 120.0f;

/// The distance the pointer drags a window by its title bar before devgui
/// offers to dock it, so that a click on the title bar never docks it.
constexpr f32 dockDragThreshold = 6.0f;

/// The longest time and distance between two presses of a double click.
constexpr f64 doubleClickTime = 0.35;
constexpr f32 doubleClickDistance = 4.0f;

/// The time before a held key starts repeating, and the time between repeats.
constexpr f64 keyRepeatDelay = 0.45;
constexpr f64 keyRepeatInterval = 0.035;

[[nodiscard]] constexpr u32
keyBit(EditKey key) noexcept
{
    return u32(1) << static_cast<u32>(key);
}

/// The keys that repeat while held.
constexpr u32 repeatableKeys =
    keyBit(EditKey::LeftArrow) | keyBit(EditKey::RightArrow) |
    keyBit(EditKey::UpArrow) | keyBit(EditKey::DownArrow) |
    keyBit(EditKey::Backspace) | keyBit(EditKey::Delete);

// NOTE: "\xE2\x80\xA6" is U+2026 HORIZONTAL ELLIPSIS in UTF-8.
constexpr std::string_view ellipsis = "\xE2\x80\xA6";

[[nodiscard]] F32Rect2D
intersect(const F32Rect2D& a, const F32Rect2D& b) noexcept
{
    const auto left = std::max(a.x, b.x);
    const auto top = std::max(a.y, b.y);
    const auto right = std::min(a.x + a.width, b.x + b.width);
    const auto bottom = std::min(a.y + a.height, b.y + b.height);
    return F32Rect2D{left, top, std::max(0.0f, right - left), std::max(0.0f, bottom - top)};
}

[[nodiscard]] bool
overlaps(const F32Rect2D& a, const F32Rect2D& b) noexcept
{
    return (a.x < b.x + b.width) && (b.x < a.x + a.width) && (a.y < b.y + b.height) && (b.y < a.y + a.height);
}

[[nodiscard]] F32Rect2D
inflate(const F32Rect2D& rect, f32 amount) noexcept
{
    return F32Rect2D{rect.x - amount, rect.y - amount, rect.width + amount * 2.0f, rect.height + amount * 2.0f};
}

[[nodiscard]] Color
withAlpha(Color color, f32 factor) noexcept
{
    color.a = static_cast<u8>(std::clamp(static_cast<f32>(color.a) * factor, 0.0f, 255.0f));
    return color;
}

[[nodiscard]] bool
hasArea(const F32Rect2D& rect) noexcept
{
    return (rect.width > 0.0f) && (rect.height > 0.0f);
}

// NOTE: A popup keeps the size of its content in one stored value.
[[nodiscard]] u64
packSize(const Vector2& size) noexcept
{
    return (static_cast<u64>(std::bit_cast<u32>(size.y)) << 32) |
           static_cast<u64>(std::bit_cast<u32>(size.x));
}

[[nodiscard]] Vector2
unpackSize(u64 value) noexcept
{
    return Vector2{
        std::bit_cast<f32>(static_cast<u32>(value & 0xFFFFFFFFULL)),
        std::bit_cast<f32>(static_cast<u32>(value >> 32)),
    };
}

/// Returns where a popup of size goes next to anchor, inside the viewport.
[[nodiscard]] F32Rect2D
placePopup(const F32Rect2D& anchor, const Vector2& size, PopupPlacement placement, const Vector2& viewport) noexcept
{
    const auto width = std::max(0.0f, std::min(size.x, viewport.x - popupMargin * 2.0f));

    if (placement == PopupPlacement::Right) {
        // NOTE: Prefer the right side of the anchor, then its left side. Clamp
        // with max after min, so that a viewport too small for the popup keeps
        // its top-left corner visible.
        auto x = anchor.x + anchor.width;
        if ((x + width > viewport.x - popupMargin) && (anchor.x - width >= popupMargin)) {
            x = anchor.x - width;
        }
        x = std::max(popupMargin, std::min(x, viewport.x - popupMargin - width));
        const auto height = std::max(0.0f, std::min(size.y, viewport.y - popupMargin * 2.0f));
        const auto y = std::max(popupMargin, std::min(anchor.y, viewport.y - popupMargin - height));
        return F32Rect2D{x, y, width, height};
    }

    // NOTE: Place the popup below the anchor, or above it when there is more
    // room above, and shrink it to fit the viewport.
    const auto x = std::max(popupMargin, std::min(anchor.x, viewport.x - popupMargin - width));
    const auto below = anchor.y + anchor.height + popupMargin;
    const auto spaceBelow = viewport.y - popupMargin - below;
    const auto spaceAbove = anchor.y - popupMargin * 2.0f;
    auto height = size.y;
    auto y = below;
    if ((height > spaceBelow) && (spaceAbove > spaceBelow)) {
        height = std::min(height, spaceAbove);
        y = anchor.y - popupMargin - height;
    }
    else {
        height = std::min(height, spaceBelow);
    }
    return F32Rect2D{x, y, width, std::max(0.0f, height)};
}

} // namespace

UI::UI(Context& context, const Input& input)
    : context_(context)
    , input_(input)
{
    POMDOG_ASSERT(!context_.frameActive_);
    context_.frameActive_ = true;
    // NOTE: Consume the request for this frame now. Requests made while
    // building it must survive until the next frame.
    context_.invalidated_ = false;

    const auto& last = context_.lastInput_;
    pressed_ = input_.primaryButtonDown && !last.primaryButtonDown;
    released_ = !input_.primaryButtonDown && last.primaryButtonDown;
    secondaryPressed_ = input_.secondaryButtonDown && !last.secondaryButtonDown;
    secondaryReleased_ = !input_.secondaryButtonDown && last.secondaryButtonDown;
    if (context_.frameCount_ > 0) {
        pointerDelta_ = input_.pointerPosition - last.pointerPosition;
    }
    if (secondaryPressed_) {
        context_.secondaryPressID_ = WidgetID{};
    }

    context_.frameCount_ += 1;
    context_.duplicateIDCount_ = 0;
    context_.focusedTextField_.reset();
    context_.frameIDs_.clear();
    context_.backgroundHotRects_.clear();
    context_.scrollStack_.clear();

    auto& drawList = context_.drawList_;
    drawList.reset(input_.viewportSize);
    drawList.beginRange(0);

    const auto viewport = F32Rect2D{0.0f, 0.0f, input_.viewportSize.x, input_.viewportSize.y};
    context_.clipStack_.clear();
    context_.clipStack_.push_back(drawList.addClipRect(viewport));

    context_.idStack_.clear();
    context_.idStack_.push_back(detail::rootID);

    // NOTE: Widgets outside any window flow from the top-left of the viewport.
    const auto& padding = context_.style_.windowPadding;
    context_.layoutStack_.clear();
    context_.layoutStack_.push_back(detail::LayoutFrame{
        .left = padding.x,
        .right = viewport.width - padding.x,
        .top = padding.y,
        .cursorX = padding.x,
        .cursorY = padding.y,
        .maxY = padding.y,
        .rowHeight = 0.0f,
        .lastItemX = padding.x,
        .requestedRight = padding.x,
        .spacing = context_.style_.itemSpacing,
        .horizontal = false,
        .hasItems = false,
    });

    // NOTE: Find the topmost window under the pointer from the rectangles of
    // the previous frame. Only widgets in that window can be hovered.
    if (input_.pointerPresent) {
        const auto& pointer = input_.pointerPosition;

        // NOTE: Dock spaces, and the windows docked in them, lie below every
        // floating window.
        hoveredWindow_ = hitTestDockSpaces(pointer);

        u64 frontDepth = 0;
        for (const auto& window : context_.windows_) {
            if (!window.docked &&
                (window.lastFrame + 1 == context_.frameCount_) &&
                (window.depth >= frontDepth) &&
                context_.getWindowHitRect(window).contains(pointer)) {
                hoveredWindow_ = window.id;
                frontDepth = window.depth;
            }
        }
    }

    updateKeyRepeat();
    updatePopupsAtFrameStart();

    // NOTE: Pressing anywhere in a floating window brings it to the front.
    if (pressed_ && !pressConsumed_ && (hoveredWindow_.value != 0)) {
        for (auto& window : context_.windows_) {
            if ((window.id == hoveredWindow_) && !window.docked) {
                context_.depthCounter_ += 1;
                window.depth = context_.depthCounter_;
                break;
            }
        }
    }
}

UI::~UI()
{
    POMDOG_ASSERT(currentWindowIndex_ < 0);
    POMDOG_ASSERT(popupLevel_ < 0);
    POMDOG_ASSERT(!inOverlay_);
    POMDOG_ASSERT(context_.idStack_.size() == 1);
    POMDOG_ASSERT(context_.layoutStack_.size() == 1);
    POMDOG_ASSERT(context_.clipStack_.size() == 1);
    POMDOG_ASSERT(context_.scrollStack_.empty());

    updateWindowDocking();
    updatePopupsAtFrameEnd();

    if ((context_.activeID_.value != 0) && !activeSeen_) {
        // NOTE: The active widget was not submitted this frame.
        context_.activeID_ = WidgetID{};
    }
    if ((context_.focusedID_.value != 0) && !focusSeen_) {
        // NOTE: The focused widget was not submitted this frame.
        context_.focusedID_ = WidgetID{};
    }
    if (context_.focusedTextField_ && (context_.focusedTextField_->id != context_.focusedID_.value)) {
        // NOTE: The field lost focus after it reported itself in this frame.
        context_.focusedTextField_.reset();
    }
    if (secondaryReleased_) {
        context_.secondaryPressID_ = WidgetID{};
    }
    for (auto& window : context_.windows_) {
        // NOTE: A close request from a tab lasts only until the window is
        // submitted in the same frame, so a window that comes back later does
        // not close at once.
        window.closeRequested = false;
    }

    context_.drawList_.finish();
    context_.hoveredWindow_ = hoveredWindow_;
    context_.hoveredID_ = hoveredID_;
    context_.hoveredRect_ = hoveredRect_;
    context_.hoveredSince_ = hoveredSince_;
    context_.wheelTarget_ = wheelCandidate_;
    context_.wakeTime_ = wakeTime_;
    context_.capture_ = InputCapture{
        .pointer = (hoveredWindow_.value != 0) || (hoveredID_.value != 0) || (context_.activeID_.value != 0),
        .wheel = wheelConsumed_,
        .keyboard = (context_.focusedID_.value != 0),
    };

    // NOTE: The text input views belong to the caller and die with this frame.
    context_.lastInput_ = input_;
    context_.lastInput_.textInput = {};
    context_.lastInput_.textInputEvents = {};
    context_.frameActive_ = false;
}

WidgetID UI::makeID(Key key) const noexcept
{
    POMDOG_ASSERT(!context_.idStack_.empty());
    return combineID(context_.idStack_.back(), key);
}

void UI::pushIDScope(Key key)
{
    context_.idStack_.push_back(makeID(key));
}

void UI::popIDScope() noexcept
{
    POMDOG_ASSERT(context_.idStack_.size() > 1);
    context_.idStack_.pop_back();
}

F32Rect2D UI::allocate(f32 width, f32 height)
{
    POMDOG_ASSERT(!context_.layoutStack_.empty());
    auto& frame = context_.layoutStack_.back();
    const auto& spacing = frame.spacing;

    F32Rect2D rect;
    if (frame.horizontal) {
        if (frame.hasItems) {
            frame.cursorX += spacing.x;
        }
        const auto w = (width > 0.0f) ? width : std::max(0.0f, frame.right - frame.cursorX);
        rect = F32Rect2D{frame.cursorX, frame.cursorY, w, height};
        frame.cursorX += w;
        frame.rowHeight = std::max(frame.rowHeight, height);
    }
    else {
        if (frame.hasItems) {
            frame.cursorY += spacing.y;
        }
        const auto w = (width > 0.0f) ? width : std::max(0.0f, frame.right - frame.left);
        rect = F32Rect2D{frame.left, frame.cursorY, w, height};
        frame.cursorY += height;
        frame.maxY = std::max(frame.maxY, frame.cursorY);
    }
    frame.hasItems = true;
    frame.lastItemX = rect.x;

    // NOTE: An item of a fixed width needs that width. An item that fills the
    // row asks for its own width with `requestWidth()`.
    if (width > 0.0f) {
        frame.requestedRight = std::max(frame.requestedRight, rect.x + width);
    }
    return rect;
}

f32 UI::getAvailableWidth() const noexcept
{
    POMDOG_ASSERT(!context_.layoutStack_.empty());
    const auto& frame = context_.layoutStack_.back();
    if (frame.horizontal) {
        const auto spacing = frame.hasItems ? frame.spacing.x : 0.0f;
        return std::max(0.0f, frame.right - frame.cursorX - spacing);
    }
    return std::max(0.0f, frame.right - frame.left);
}

bool UI::isLayoutHorizontal() const noexcept
{
    POMDOG_ASSERT(!context_.layoutStack_.empty());
    return context_.layoutStack_.back().horizontal;
}

void UI::beginHorizontal()
{
    POMDOG_ASSERT(!context_.layoutStack_.empty());
    const auto& parent = context_.layoutStack_.back();
    POMDOG_ASSERT(!parent.horizontal);

    const auto y = parent.cursorY + (parent.hasItems ? parent.spacing.y : 0.0f);
    const auto row = detail::LayoutFrame{
        .left = parent.left,
        .right = parent.right,
        .top = y,
        .cursorX = parent.left,
        .cursorY = y,
        .maxY = y,
        .rowHeight = 0.0f,
        .lastItemX = parent.left,
        .requestedRight = parent.left,
        .spacing = context_.style_.itemSpacing,
        .horizontal = true,
        .hasItems = false,
    };
    context_.layoutStack_.push_back(row);
}

void UI::endHorizontal()
{
    POMDOG_ASSERT(context_.layoutStack_.size() > 1);
    const auto row = context_.layoutStack_.back();
    POMDOG_ASSERT(row.horizontal);
    context_.layoutStack_.pop_back();

    if (row.hasItems) {
        // NOTE: The row takes the whole width, but asks only for what its
        // items asked for.
        const auto requestedRight = context_.layoutStack_.back().requestedRight;
        [[maybe_unused]] const auto rowRect = allocate(row.right - row.left, row.rowHeight);
        auto& parent = context_.layoutStack_.back();
        parent.requestedRight = std::max(requestedRight, row.requestedRight);
    }
}

void UI::beginVertical(f32 spacing)
{
    POMDOG_ASSERT(!context_.layoutStack_.empty());
    const auto& parent = context_.layoutStack_.back();
    POMDOG_ASSERT(!parent.horizontal);

    const auto y = parent.cursorY + (parent.hasItems ? parent.spacing.y : 0.0f);
    const auto column = detail::LayoutFrame{
        .left = parent.left,
        .right = parent.right,
        .top = y,
        .cursorX = parent.left,
        .cursorY = y,
        .maxY = y,
        .rowHeight = 0.0f,
        .lastItemX = parent.left,
        .requestedRight = parent.left,
        .spacing = Vector2{context_.style_.itemSpacing.x, spacing},
        .horizontal = false,
        .hasItems = false,
    };
    context_.layoutStack_.push_back(column);
}

void UI::endVertical()
{
    POMDOG_ASSERT(context_.layoutStack_.size() > 1);
    const auto column = context_.layoutStack_.back();
    POMDOG_ASSERT(!column.horizontal);
    context_.layoutStack_.pop_back();

    if (column.hasItems) {
        // NOTE: The column takes the whole width, but asks only for what its
        // items asked for.
        const auto requestedRight = context_.layoutStack_.back().requestedRight;
        [[maybe_unused]] const auto columnRect = allocate(column.right - column.left, column.maxY - column.top);
        auto& parent = context_.layoutStack_.back();
        parent.requestedRight = std::max(requestedRight, column.requestedRight);
    }
}

void UI::indent(f32 amount) noexcept
{
    POMDOG_ASSERT(!context_.layoutStack_.empty());
    auto& frame = context_.layoutStack_.back();
    frame.left += amount;
    if (!frame.horizontal) {
        frame.cursorX = frame.left;
    }
}

void UI::requestWidth(f32 width) noexcept
{
    POMDOG_ASSERT(!context_.layoutStack_.empty());
    auto& frame = context_.layoutStack_.back();
    frame.requestedRight = std::max(frame.requestedRight, frame.lastItemX + width);
}

Response UI::interact(WidgetID id, const F32Rect2D& rect)
{
    return resolveInteraction(id, rect, false);
}

Response UI::interactBehind(WidgetID id, const F32Rect2D& rect)
{
    return resolveInteraction(id, rect, true);
}

Response UI::resolveInteraction(WidgetID id, const F32Rect2D& rect, bool behind)
{
    Response response;
    response.id = id;
    response.rect = rect;

    if (!context_.frameIDs_.insert(id)) {
        context_.duplicateIDCount_ += 1;
        strokeRect(rect, context_.style_.errorColor, 1.0f);
        return response;
    }

    if (context_.focusedID_ == id) {
        focusSeen_ = true;
    }

    const bool pointerOver = isPointerOverCurrentLayer() && rect.contains(input_.pointerPosition);

    auto& activeID = context_.activeID_;
    if (activeID == id) {
        activeSeen_ = true;
    }

    // NOTE: Widgets in front are submitted later, so the last hovered widget
    // of the previous frame is the frontmost one. A widget behind leaves the
    // press to it while the pointer is still over it, and stays hovered while
    // the pointer holds it, so that the caller keeps submitting it.
    const bool overFront = behind &&
                           (context_.hoveredID_.value != 0) &&
                           (context_.hoveredID_ != id) &&
                           context_.hoveredRect_.contains(input_.pointerPosition);
    const bool frontActive = overFront && (activeID == context_.hoveredID_);

    response.hovered = pointerOver && ((activeID.value == 0) || (activeID == id) || frontActive);
    if (response.hovered) {
        // NOTE: Keep the time the pointer arrived while it stays on one widget.
        hoveredSince_ = (context_.hoveredID_ == id) ? context_.hoveredSince_ : input_.timeSeconds;
        hoveredID_ = id;
        hoveredRect_ = rect;
    }

    // NOTE: Like a click, a secondary click needs the press and the release on
    // the same widget. The first widget under the pointer takes the press.
    auto& secondaryPressID = context_.secondaryPressID_;
    if (response.hovered && secondaryPressed_ && (secondaryPressID.value == 0) && !overFront) {
        secondaryPressID = id;
    }
    if (response.hovered && secondaryReleased_ && (secondaryPressID == id) && (activeID.value == 0)) {
        response.secondaryClicked = true;
    }

    if (response.hovered && pressed_ && !pressConsumed_ && !overFront) {
        activeID = id;
        activeSeen_ = true;
        pressConsumed_ = true;
        context_.activeScratch_ = 0.0f;
        response.activated = true;

        const auto distance = input_.pointerPosition - context_.lastPressPosition_;
        response.doubleClicked = (context_.lastPressID_ == id) &&
                                 (input_.timeSeconds - context_.lastPressTime_ <= doubleClickTime) &&
                                 (std::abs(distance.x) <= doubleClickDistance) &&
                                 (std::abs(distance.y) <= doubleClickDistance);

        // NOTE: A third press starts a new double click instead of making one.
        context_.lastPressID_ = response.doubleClicked ? WidgetID{} : id;
        context_.lastPressTime_ = input_.timeSeconds;
        context_.lastPressPosition_ = input_.pointerPosition;
    }

    if (activeID == id) {
        if (released_) {
            response.deactivated = true;
            response.clicked = pointerOver;
            activeID = WidgetID{};
        }
        else {
            response.active = true;
        }
    }

    if (currentWindow_.value == 0) {
        context_.backgroundHotRects_.push_back(intersect(rect, getClipRect()));
    }
    return response;
}

bool UI::isPointerPressed() const noexcept
{
    return pressed_;
}

Vector2 UI::getPointerDelta() const noexcept
{
    return pointerDelta_;
}

Vector2 UI::getPressPosition() const noexcept
{
    return context_.lastPressPosition_;
}

f64 UI::getHoveredDuration(WidgetID id) const noexcept
{
    if ((id.value == 0) || (hoveredID_ != id)) {
        return 0.0;
    }
    return input_.timeSeconds - hoveredSince_;
}

f32& UI::getActiveScratch() noexcept
{
    return context_.activeScratch_;
}

bool UI::isKeyPressed(EditKey key) const noexcept
{
    return (keysPressed_ & keyBit(key)) != 0;
}

void UI::setKeyboardFocus(WidgetID id) noexcept
{
    context_.focusedID_ = id;
    focusSeen_ = true;
}

void UI::clearKeyboardFocus() noexcept
{
    context_.focusedID_ = WidgetID{};
}

bool UI::hasKeyboardFocus(WidgetID id) const noexcept
{
    return (id.value != 0) && (context_.focusedID_ == id);
}

TextEditState& UI::getTextEditState() noexcept
{
    return context_.textEdit_;
}

raw_ptr<clipboard::ClipboardService> UI::getClipboard() noexcept
{
    return context_.clipboard_.get();
}

void UI::setFocusedTextField(const textinput::TextFieldState& field)
{
    POMDOG_ASSERT(field.id != 0);

    // NOTE: The caller's text may not outlive the frame. Keep a copy that
    // stays valid until the next frame starts.
    context_.focusedTextFieldText_.assign(field.surroundingText);
    auto& stored = context_.focusedTextField_.emplace(field);
    stored.surroundingText = context_.focusedTextFieldText_;
}

std::optional<u64> UI::loadState(WidgetID id) noexcept
{
    return context_.states_.find(id);
}

void UI::storeState(WidgetID id, u64 value) noexcept
{
    context_.states_.store(id, value);
}

void UI::fillRect(const F32Rect2D& rect, const Color& color, f32 radius)
{
    if ((color.a == 0) || !isVisible(rect)) {
        return;
    }
    DrawCommand command;
    command.kind = DrawCommandKind::FillRect;
    command.p0 = Vector2{rect.x, rect.y};
    command.p1 = Vector2{rect.width, rect.height};
    command.color = color;
    command.radius = std::min({radius, rect.width * 0.5f, rect.height * 0.5f});
    addCommand(command);
}

void UI::strokeRect(const F32Rect2D& rect, const Color& color, f32 thickness, f32 radius)
{
    if ((color.a == 0) || !isVisible(rect)) {
        return;
    }
    DrawCommand command;
    command.kind = DrawCommandKind::StrokeRect;
    command.p0 = Vector2{rect.x, rect.y};
    command.p1 = Vector2{rect.width, rect.height};
    command.color = color;
    command.thickness = thickness;
    command.radius = std::min({radius, rect.width * 0.5f, rect.height * 0.5f});
    addCommand(command);
}

void UI::fillRectGradient(const F32Rect2D& rect, const RectGradient& gradient)
{
    if (!isVisible(rect)) {
        return;
    }
    DrawCommand command;
    command.kind = DrawCommandKind::FillRectGradient;
    command.p0 = Vector2{rect.x, rect.y};
    command.p1 = Vector2{rect.width, rect.height};
    command.clipIndex = context_.clipStack_.back();
    context_.drawList_.addCommand(command, gradient);
}

void UI::fillCircle(const Vector2& center, f32 radius, const Color& color)
{
    if ((color.a == 0) || !isVisible(F32Rect2D{center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f})) {
        return;
    }
    DrawCommand command;
    command.kind = DrawCommandKind::FillCircle;
    command.p0 = center;
    command.color = color;
    command.radius = radius;
    addCommand(command);
}

void UI::fillTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& color)
{
    const auto minX = std::min({p0.x, p1.x, p2.x});
    const auto minY = std::min({p0.y, p1.y, p2.y});
    const auto maxX = std::max({p0.x, p1.x, p2.x});
    const auto maxY = std::max({p0.y, p1.y, p2.y});
    if ((color.a == 0) || !isVisible(F32Rect2D{minX, minY, maxX - minX, maxY - minY})) {
        return;
    }
    DrawCommand command;
    command.kind = DrawCommandKind::FillTriangle;
    command.p0 = p0;
    command.p1 = p1;
    command.p2 = p2;
    command.color = color;
    addCommand(command);
}

void UI::drawLine(const Vector2& start, const Vector2& end, const Color& color, f32 thickness)
{
    const auto minX = std::min(start.x, end.x) - thickness;
    const auto minY = std::min(start.y, end.y) - thickness;
    const auto maxX = std::max(start.x, end.x) + thickness;
    const auto maxY = std::max(start.y, end.y) + thickness;
    if ((color.a == 0) || !isVisible(F32Rect2D{minX, minY, maxX - minX, maxY - minY})) {
        return;
    }
    DrawCommand command;
    command.kind = DrawCommandKind::Line;
    command.p0 = start;
    command.p1 = end;
    command.color = color;
    command.thickness = thickness;
    addCommand(command);
}

void UI::drawPolyline(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& color, f32 thickness)
{
    const auto minX = std::min({p0.x, p1.x, p2.x}) - thickness;
    const auto minY = std::min({p0.y, p1.y, p2.y}) - thickness;
    const auto maxX = std::max({p0.x, p1.x, p2.x}) + thickness;
    const auto maxY = std::max({p0.y, p1.y, p2.y}) + thickness;
    if ((color.a == 0) || !isVisible(F32Rect2D{minX, minY, maxX - minX, maxY - minY})) {
        return;
    }
    DrawCommand command;
    command.kind = DrawCommandKind::Polyline;
    command.p0 = p0;
    command.p1 = p1;
    command.p2 = p2;
    command.color = color;
    command.thickness = thickness;
    addCommand(command);
}

void UI::drawText(const Vector2& position, std::string_view text, const Color& color)
{
    if (text.empty() || (color.a == 0)) {
        return;
    }

    // NOTE: Cull with the line height and the clip's right edge alone, so that
    // culling does not need the text width.
    const auto& clip = getClipRect();
    const auto fontSize = context_.style_.fontSize;
    if ((position.y >= clip.y + clip.height) || (position.y + fontSize <= clip.y) || (position.x >= clip.x + clip.width)) {
        return;
    }

    DrawCommand command;
    command.kind = DrawCommandKind::Text;
    command.p0 = position;
    command.color = color;
    command.fontSize = fontSize;
    addCommand(command, text);
}

void UI::drawText(const Vector2& position, std::string_view text, const Color& color, f32 maxWidth)
{
    if (text.empty()) {
        return;
    }
    if (measureText(text) <= maxWidth) {
        drawText(position, text, color);
        return;
    }

    const auto ellipsisWidth = measureText(ellipsis);
    const auto measure = [this](std::string_view s) -> f32 { return measureText(s); };
    const auto length = detail::fitTextWithEllipsis(measure, text, maxWidth, ellipsisWidth);
    const auto prefix = text.substr(0, length);
    drawText(position, prefix, color);
    drawText(Vector2{position.x + measureText(prefix), position.y}, ellipsis, color);
}

f32 UI::measureText(std::string_view text)
{
    if (text.empty()) {
        return 0.0f;
    }
    return context_.textMeasurer_->measureWidth(text, context_.style_.fontSize);
}

std::string_view UI::wrapText(WidgetID id, std::string_view text, f32 maxWidth, i18n::LineBreakLanguage language)
{
    const auto fontSize = context_.style_.fontSize;
    auto& entries = context_.wrappedTexts_;

    // NOTE: Never overwrite a result already returned in this frame, even
    // when the same widget asks for different text or wrapping parameters.
    detail::WrappedTextEntry* target = nullptr;
    for (auto& entry : entries) {
        const bool unchanged = (entry.id == id) &&
                               (entry.maxWidth == maxWidth) &&
                               (entry.fontSize == fontSize) &&
                               (entry.language == language) &&
                               (entry.sourceText == text);
        if (unchanged) {
            entry.lastUsedFrame = context_.frameCount_;
            return entry.wrappedText;
        }
        if ((entry.lastUsedFrame != context_.frameCount_) &&
            ((target == nullptr) || (entry.lastUsedFrame < target->lastUsedFrame))) {
            target = &entry;
        }
    }
    if (target == nullptr) {
        target = &entries.emplace_back();
    }
    auto& entry = *target;
    const auto measure = [this](std::string_view s) -> f32 { return measureText(s); };
    auto result = i18n::wrapText(measure, text, maxWidth, language);
    entry.id = id;
    entry.maxWidth = maxWidth;
    entry.fontSize = fontSize;
    entry.language = language;
    entry.sourceText.assign(text);
    entry.wrappedText = std::move(result.wrappedText);
    entry.lastUsedFrame = context_.frameCount_;
    return entry.wrappedText;
}

void UI::pushClipRect(const F32Rect2D& rect)
{
    const auto clipped = intersect(rect, getClipRect());
    context_.clipStack_.push_back(context_.drawList_.addClipRect(clipped));
}

void UI::popClipRect() noexcept
{
    POMDOG_ASSERT(context_.clipStack_.size() > 1);
    context_.clipStack_.pop_back();
}

bool UI::isVisible(const F32Rect2D& rect) const noexcept
{
    return overlaps(rect, getClipRect());
}

bool UI::beginWindow(Key key, std::string_view title, const F32Rect2D& initialRect, bool initiallyCollapsed, raw_ptr<bool> open)
{
    // NOTE: Windows cannot nest, and cannot open in a popup or an overlay. Use a
    // scroll area for a nested region.
    POMDOG_ASSERT(currentWindowIndex_ < 0);
    POMDOG_ASSERT((popupLevel_ < 0) && !inOverlay_);

    // NOTE: Window IDs ignore the ID scopes, so that dock layouts and saved
    // layouts can name windows by key alone.
    const auto id = combineID(detail::rootID, key);
    auto& windows = context_.windows_;
    auto iter = std::find_if(windows.begin(), windows.end(), [&](const detail::WindowState& w) { return w.id == id; });
    if (iter == windows.end()) {
        context_.depthCounter_ += 1;
        detail::WindowState state;
        state.id = id;
        state.key = key;
        state.depth = context_.depthCounter_;
        state.hasRect = false;
        windows.push_back(std::move(state));
        iter = std::prev(windows.end());
    }
    if (!iter->hasRect) {
        iter->rect = initialRect;
        iter->collapsed = initiallyCollapsed;
        iter->hasRect = true;
    }

    // NOTE: A window submitted twice in one frame would draw over itself.
    POMDOG_ASSERT(iter->lastFrame != context_.frameCount_);
    const bool submittedLastFrame = (iter->lastFrame + 1 == context_.frameCount_);
    iter->lastFrame = context_.frameCount_;
    if (iter->title != title) {
        iter->title.assign(title);
    }
    iter->closable = (open != nullptr);
    if (iter->closeRequested) {
        // NOTE: The user clicked the close button on the window's tab earlier
        // in this frame.
        iter->closeRequested = false;
        if (open != nullptr) {
            *open = false;
        }
    }

    currentWindowIndex_ = static_cast<i32>(std::distance(windows.begin(), iter));
    currentWindow_ = id;
    context_.idStack_.push_back(id);

    const auto& style = context_.style_;

    std::size_t dockSpaceIndex = 0;
    i32 nodeIndex = -1;
    if (findDockedNode(id, dockSpaceIndex, nodeIndex)) {
        iter->docked = true;
        if (!submittedLastFrame) {
            // NOTE: The dock space built its tab bar before this window came
            // back. Ask for one more frame so that the tab bar shows it.
            requestUpdateAt(input_.timeSeconds);
        }

        const auto& dockSpace = context_.dockSpaces_[dockSpaceIndex];
        const auto& node = dockSpace.nodes[static_cast<std::size_t>(nodeIndex)];
        if ((dockSpace.lastFrame != context_.frameCount_) || (node.selectedTab != id)) {
            windowMode_ = WindowMode::Hidden;
            return false;
        }

        // NOTE: Docked windows draw at the depth of the dock space, after it,
        // so they lie below every floating window.
        windowMode_ = WindowMode::Docked;
        currentDepth_ = 0;
        context_.drawList_.beginRange(0);
        beginScrollRegion(makeID(windowScrollKey), node.contentRect, iter->scrollY, iter->contentHeight, style.windowPadding);
        windowContentOpen_ = true;
        return true;
    }

    iter->docked = false;
    windowMode_ = WindowMode::Floating;
    currentDepth_ = iter->depth;
    context_.drawList_.beginRange(iter->depth);

    const auto titleHeight = style.titleBarHeight;

    // NOTE: Process the controls on the frame before the content, so they take
    // the press before any content widget underneath them.
    {
        const auto& rect = iter->rect;
        const auto collapseRect = F32Rect2D{rect.x, rect.y, titleHeight, titleHeight};
        if (interact(makeID(windowCollapseKey), collapseRect).clicked) {
            iter->collapsed = !iter->collapsed;
        }
    }
    Response closeResponse;
    if (open != nullptr) {
        const auto& rect = iter->rect;
        const auto closeRect = F32Rect2D{rect.x + rect.width - titleHeight, rect.y, titleHeight, titleHeight};
        closeResponse = interact(makeID(windowCloseKey), closeRect);
        if (closeResponse.clicked) {
            *open = false;
            requestUpdateAt(input_.timeSeconds);
        }
    }
    {
        const auto titleRect = F32Rect2D{iter->rect.x, iter->rect.y, iter->rect.width, titleHeight};
        const auto titleResponse = interact(makeID(windowTitleKey), titleRect);
        if (titleResponse.isHeld() && !titleResponse.activated) {
            iter->rect.x += pointerDelta_.x;
            iter->rect.y += pointerDelta_.y;

            auto& travel = context_.activeScratch_;
            travel += std::abs(pointerDelta_.x) + std::abs(pointerDelta_.y);
            if (travel >= dockDragThreshold) {
                draggedWindow_ = id;
            }
        }
    }
    gripHovered_ = false;
    if (!iter->collapsed) {
        const auto& rect = iter->rect;
        const auto gripRect = F32Rect2D{
            rect.x + rect.width - style.resizeGripSize,
            rect.y + rect.height - style.resizeGripSize,
            style.resizeGripSize,
            style.resizeGripSize,
        };
        const auto gripResponse = interact(makeID(windowGripKey), gripRect);
        if (gripResponse.isHeld() && !gripResponse.activated) {
            iter->rect.width = std::max(style.minWindowSize.x, iter->rect.width + pointerDelta_.x);
            iter->rect.height = std::max(style.minWindowSize.y, iter->rect.height + pointerDelta_.y);
        }
        gripHovered_ = gripResponse.hovered || gripResponse.active;
    }

    // NOTE: Keep the title bar reachable so a window cannot be lost off screen.
    {
        auto& rect = iter->rect;
        // NOTE: Prefer the left and top limits when the viewport is too small
        // for both, so `std::clamp` with lo > hi is never called.
        const auto maxX = input_.viewportSize.x - windowGrabMargin;
        const auto maxY = input_.viewportSize.y - titleHeight;
        rect.x = std::max(windowGrabMargin - rect.width, std::min(rect.x, maxX));
        rect.y = std::max(0.0f, std::min(rect.y, maxY));
    }

    const bool focused = (iter->depth == context_.depthCounter_);
    const auto& rect = iter->rect;
    const auto titleRect = F32Rect2D{rect.x, rect.y, rect.width, titleHeight};
    const auto frameRect = iter->collapsed ? titleRect : rect;
    const auto rounding = style.windowRounding;

    // NOTE: A soft shadow made of a few translucent layers below the window.
    for (i32 i = 1; i <= 3; ++i) {
        const auto spread = static_cast<f32>(i);
        auto shadowRect = inflate(frameRect, spread);
        shadowRect.y += 2.0f;
        fillRect(shadowRect, withAlpha(style.shadowColor, 1.0f / spread), rounding + spread);
    }

    if (iter->collapsed) {
        fillRect(titleRect, style.titleBarColor, rounding);
    }
    else {
        fillRect(rect, style.windowBackgroundColor, rounding);
        fillRect(titleRect, style.titleBarColor, rounding);

        // NOTE: Square off the bottom corners of the title bar and separate it
        // from the content with a line.
        const auto squareHeight = std::min(rounding, titleHeight);
        fillRect(F32Rect2D{rect.x, rect.y + titleHeight - squareHeight, rect.width, squareHeight}, style.titleBarColor);
        fillRect(F32Rect2D{rect.x, rect.y + titleHeight - 1.0f, rect.width, 1.0f}, style.borderColor);
    }

    const auto textColor = focused ? style.textColor : style.textSecondaryColor;
    {
        // NOTE: A chevron that points right when collapsed and down when expanded.
        const auto center = Vector2{rect.x + titleHeight * 0.5f, rect.y + titleHeight * 0.5f};
        const auto r = std::round(style.fontSize * 0.22f);
        if (iter->collapsed) {
            drawPolyline(Vector2{center.x - r * 0.5f, center.y - r}, Vector2{center.x + r * 0.5f, center.y}, Vector2{center.x - r * 0.5f, center.y + r}, textColor, 1.5f);
        }
        else {
            drawPolyline(Vector2{center.x - r, center.y - r * 0.5f}, Vector2{center.x, center.y + r * 0.5f}, Vector2{center.x + r, center.y - r * 0.5f}, textColor, 1.5f);
        }
    }

    const auto closeWidth = (open != nullptr) ? titleHeight : style.framePadding.x;
    const auto titleTextX = rect.x + titleHeight;
    const auto titleTextWidth = std::max(0.0f, rect.width - titleHeight - closeWidth);
    drawText(Vector2{titleTextX, rect.y + (titleHeight - style.fontSize) * 0.5f}, title, textColor, titleTextWidth);
    if (open != nullptr) {
        detail::drawCloseButton(*this, closeResponse.rect, closeResponse, true);
    }

    if (iter->collapsed) {
        strokeRect(titleRect, focused ? style.inputBorderColor : style.borderColor, 1.0f, rounding);
        return false;
    }

    const auto contentRect = F32Rect2D{rect.x, rect.y + titleHeight, rect.width, std::max(0.0f, rect.height - titleHeight)};
    beginScrollRegion(makeID(windowScrollKey), contentRect, iter->scrollY, iter->contentHeight, style.windowPadding);
    windowContentOpen_ = true;
    return true;
}

void UI::endWindow()
{
    POMDOG_ASSERT(currentWindowIndex_ >= 0);
    POMDOG_ASSERT(static_cast<std::size_t>(currentWindowIndex_) < context_.windows_.size());

    auto& window = context_.windows_[static_cast<std::size_t>(currentWindowIndex_)];
    const auto& style = context_.style_;

    if (windowContentOpen_) {
        const auto result = endScrollRegion();
        window.scrollY = result.scrollY;
        window.contentHeight = result.contentHeight;
    }

    if ((windowMode_ == WindowMode::Floating) && !window.collapsed) {
        // NOTE: A resize grip of two diagonal strokes in the bottom-right corner.
        const auto& rect = window.rect;
        const auto right = rect.x + rect.width - 4.0f;
        const auto bottom = rect.y + rect.height - 4.0f;
        const auto gripColor = gripHovered_ ? style.textColor : style.inputHoveredBorderColor;
        drawLine(Vector2{right - 8.0f, bottom}, Vector2{right, bottom - 8.0f}, gripColor, 1.0f);
        drawLine(Vector2{right - 4.0f, bottom}, Vector2{right, bottom - 4.0f}, gripColor, 1.0f);

        const bool focused = (window.depth == context_.depthCounter_);
        strokeRect(rect, focused ? style.inputBorderColor : style.borderColor, 1.0f, style.windowRounding);
    }

    if (windowMode_ != WindowMode::Hidden) {
        // NOTE: A press on the window background makes the window itself
        // active, so that dragging across other widgets does not hover them.
        auto& activeID = context_.activeID_;
        if (pressed_ && !pressConsumed_ && (hoveredWindow_ == window.id)) {
            activeID = window.id;
            pressConsumed_ = true;
        }
        if (activeID == window.id) {
            activeSeen_ = true;
            if (released_) {
                activeID = WidgetID{};
            }
        }
    }

    context_.idStack_.pop_back();
    context_.drawList_.beginRange(0);
    currentWindowIndex_ = -1;
    currentWindow_ = WidgetID{};
    currentDepth_ = 0;
    windowMode_ = WindowMode::Floating;
    windowContentOpen_ = false;
}

void UI::startWindowDrag(std::size_t windowIndex, const Vector2& grabOffset)
{
    POMDOG_ASSERT(windowIndex < context_.windows_.size());
    auto& window = context_.windows_[windowIndex];
    window.docked = false;
    window.collapsed = false;

    // NOTE: The title bar adds this frame's pointer movement again when the
    // window is submitted later in the frame, so leave it out here.
    window.rect.x = input_.pointerPosition.x - grabOffset.x - pointerDelta_.x;
    window.rect.y = input_.pointerPosition.y - grabOffset.y - pointerDelta_.y;
    context_.depthCounter_ += 1;
    window.depth = context_.depthCounter_;

    // NOTE: The drag already went far enough, so the window offers to dock
    // from its first frame.
    context_.activeID_ = combineID(window.id, windowTitleKey);
    context_.activeScratch_ = dockDragThreshold;
}

void UI::openPopup(WidgetID id, PopupOutsidePress outsidePress)
{
    POMDOG_ASSERT(id.value != 0);
    auto& popups = context_.popups_;
    const auto parentCount = static_cast<std::size_t>(popupLevel_ + 1);
    if (popups.size() < parentCount) {
        // NOTE: The popup being built closed earlier in this frame, so it
        // cannot open a child.
        return;
    }
    if ((popups.size() > parentCount) && (popups[parentCount].id == id)) {
        return;
    }
    POMDOG_ASSERT(parentCount < maxPopupDepth);
    if (parentCount >= maxPopupDepth) {
        return;
    }

    popups.resize(parentCount);
    detail::PopupState popup;
    popup.id = id;
    popup.openPosition = input_.pointerPosition;
    popup.openedThisFrame = true;
    popup.consumeOutsidePress = (outsidePress == PopupOutsidePress::Consume);
    popups.push_back(popup);

    // NOTE: The popup may be built before this call or not at all in this
    // frame. Ask for one more frame so that it appears without more input.
    requestUpdateAt(input_.timeSeconds);
}

void UI::closePopup() noexcept
{
    if (!context_.popups_.empty()) {
        context_.popups_.clear();

        // NOTE: Some popups may already be drawn in this frame.
        requestUpdateAt(input_.timeSeconds);
    }
}

void UI::closeChildPopups() noexcept
{
    auto& popups = context_.popups_;
    const auto keep = static_cast<std::size_t>(popupLevel_ + 1);
    if (popups.size() > keep) {
        popups.resize(keep);
        requestUpdateAt(input_.timeSeconds);
    }
}

bool UI::isPointerOverPopup(WidgetID id) const noexcept
{
    return (id.value != 0) && (hoveredWindow_ == id);
}

bool UI::isPopupOpen(WidgetID id) const noexcept
{
    if (id.value == 0) {
        return false;
    }
    const auto& popups = context_.popups_;
    return std::any_of(popups.begin(), popups.end(), [&](const detail::PopupState& p) { return p.id == id; });
}

bool UI::hasChildPopups() const noexcept
{
    return context_.popups_.size() > static_cast<std::size_t>(popupLevel_ + 1);
}

Vector2 UI::getPopupOpenPosition(WidgetID id) const noexcept
{
    for (const auto& popup : context_.popups_) {
        if (popup.id == id) {
            return popup.openPosition;
        }
    }
    return input_.pointerPosition;
}

void UI::beginPopup(WidgetID id, const F32Rect2D& anchor, const Vector2& size, PopupPlacement placement)
{
    POMDOG_ASSERT(!inOverlay_);
    const auto level = static_cast<std::size_t>(popupLevel_ + 1);
    POMDOG_ASSERT(level < maxPopupDepth);

    // NOTE: A popup opened while building another one is its child, so it
    // must be built inside that popup.
    auto& popups = context_.popups_;
    POMDOG_ASSERT(level < popups.size());
    POMDOG_ASSERT(popups[level].id == id);
    auto& popup = popups[level];
    popup.submitted = true;

    const auto& style = context_.style_;
    const auto& viewport = input_.viewportSize;
    auto& frame = popupFrames_[level];
    frame.id = id;
    frame.measuredSize = std::nullopt;
    if (const auto stored = loadState(id); stored) {
        frame.measuredSize = unpackSize(*stored);
    }

    const bool fitWidth = (size.x <= 0.0f);
    const bool fitHeight = (size.y <= 0.0f);
    frame.measuring = !frame.measuredSize && (fitWidth || fitHeight);

    const auto measured = frame.measuredSize.value_or(Vector2{0.0f, 0.0f});
    auto desired = size;
    if (fitWidth) {
        desired.x = frame.measuring ? viewport.x : std::max(measured.x, minFitPopupWidth);
    }
    if (fitHeight) {
        desired.y = frame.measuring ? viewport.y : measured.y;
    }
    frame.rect = placePopup(anchor, desired, placement, viewport);
    popup.rect = frame.measuring ? F32Rect2D{frame.rect.x, frame.rect.y, 0.0f, 0.0f} : frame.rect;

    // NOTE: Each nesting level draws above its parent and below overlays.
    frame.saved = enterLayer(id, overlayDepth - maxPopupDepth + level);
    popupLevel_ = static_cast<i32>(level);
    context_.idStack_.push_back(id);

    if (frame.measuring) {
        // NOTE: Lay out the content once without showing it or taking input,
        // to learn its size for the next frame.
        context_.clipStack_.push_back(context_.drawList_.addClipRect(F32Rect2D{0.0f, 0.0f, 0.0f, 0.0f}));
    }
    else {
        for (i32 i = 1; i <= 3; ++i) {
            const auto spread = static_cast<f32>(i);
            auto shadowRect = inflate(frame.rect, spread);
            shadowRect.y += 2.0f;
            fillRect(shadowRect, withAlpha(style.shadowColor, 1.0f / spread), style.popupRounding + spread);
        }
        fillRect(frame.rect, style.popupBackgroundColor, style.popupRounding);
    }

    beginScrollRegion(makeID(popupScrollKey), frame.rect, popup.scrollY, frame.measuring ? 0.0f : measured.y, style.popupPadding);
}

void UI::endPopup()
{
    POMDOG_ASSERT(popupLevel_ >= 0);
    const auto level = static_cast<std::size_t>(popupLevel_);
    const auto& frame = popupFrames_[level];
    const auto id = frame.id;
    const auto& style = context_.style_;

    const auto result = endScrollRegion();

    // NOTE: The content may have closed the popup, for example when the user
    // chose an item.
    auto& popups = context_.popups_;
    if ((level < popups.size()) && (popups[level].id == id)) {
        popups[level].scrollY = result.scrollY;
    }

    const auto contentSize = Vector2{std::ceil(result.contentWidth), result.contentHeight};
    if (!frame.measuredSize || (*frame.measuredSize != contentSize)) {
        storeState(id, packSize(contentSize));

        // NOTE: This frame was laid out with the previous size. Ask for one
        // more frame so the popup settles even when no input arrives.
        requestUpdateAt(input_.timeSeconds);
    }

    if (frame.measuring) {
        popClipRect();
    }
    else {
        strokeRect(frame.rect, style.popupBorderColor, 1.0f, style.popupRounding);
    }

    // NOTE: A press on the popup background does nothing but keeps the popup.
    auto& activeID = context_.activeID_;
    if (pressed_ && !pressConsumed_ && (hoveredWindow_ == id)) {
        activeID = id;
        pressConsumed_ = true;
    }
    if (activeID == id) {
        activeSeen_ = true;
        if (released_) {
            activeID = WidgetID{};
        }
    }

    context_.idStack_.pop_back();
    popupLevel_ -= 1;
    leaveLayer(frame.saved);
}

void UI::updatePopupsAtFrameStart()
{
    auto& popups = context_.popups_;
    if (popups.empty()) {
        return;
    }

    // NOTE: Popups lie above every window, and each popup above its parent.
    auto hoveredLevel = popups.size();
    if (input_.pointerPresent) {
        for (auto i = popups.size(); i > 0; --i) {
            const auto& rect = popups[i - 1].rect;
            if (hasArea(rect) && rect.contains(input_.pointerPosition)) {
                hoveredLevel = i - 1;
                break;
            }
        }
    }
    if (hoveredLevel < popups.size()) {
        hoveredWindow_ = popups[hoveredLevel].id;
    }

    if (pressed_ || secondaryPressed_) {
        if (hoveredLevel < popups.size()) {
            // NOTE: A press in a popup closes the popups opened from it.
            popups.resize(hoveredLevel + 1);
        }
        else {
            // NOTE: A press outside every popup closes them all. A primary
            // press does nothing else, unless every popup lets it through,
            // but a secondary press can still open another context menu where
            // it is released.
            const bool consume = std::any_of(popups.begin(), popups.end(), [](const detail::PopupState& p) { return p.consumeOutsidePress; });
            popups.clear();
            pressConsumed_ = pressed_ && consume;
        }
    }

    if (!popups.empty() && (context_.focusedID_.value == 0) && isKeyPressed(EditKey::Escape)) {
        // NOTE: Escape closes the innermost popup.
        popups.pop_back();
    }
}

void UI::updatePopupsAtFrameEnd()
{
    // NOTE: A popup that this frame did not submit belongs to a widget that is
    // gone, so it closes with the popups opened from it.
    auto& popups = context_.popups_;
    for (std::size_t i = 0; i < popups.size(); ++i) {
        if (!popups[i].submitted && !popups[i].openedThisFrame) {
            popups.resize(i);
            break;
        }
    }
    for (auto& popup : popups) {
        popup.submitted = false;
        popup.openedThisFrame = false;
    }
}

void UI::beginOverlay()
{
    POMDOG_ASSERT(!inOverlay_);
    overlaySaved_ = enterLayer(overlayLayerID, overlayDepth);
    inOverlay_ = true;
}

void UI::endOverlay()
{
    POMDOG_ASSERT(inOverlay_);
    inOverlay_ = false;
    leaveLayer(overlaySaved_);
}

void UI::beginScrollRegion(WidgetID id, const F32Rect2D& viewRect, f32 scrollY, f32 lastContentHeight, const Vector2& padding)
{
    const auto& style = context_.style_;
    const bool hasScrollbar = lastContentHeight > viewRect.height;
    const auto maxScroll = std::max(0.0f, lastContentHeight - viewRect.height);

    const bool pointerInView = isPointerOverCurrentLayer() && viewRect.contains(input_.pointerPosition);
    if (pointerInView && (input_.wheelDelta.y != 0.0f) && !wheelConsumed_ && (context_.wheelTarget_ == id)) {
        scrollY -= input_.wheelDelta.y * style.scrollStep;
        wheelConsumed_ = true;
    }
    if (pointerInView && hasScrollbar) {
        // NOTE: The wheel goes to the innermost scrollable region under the
        // pointer. Inner regions begin after outer ones and overwrite this,
        // and the next frame applies the wheel to the region found here.
        wheelCandidate_ = id;
    }
    scrollY = std::clamp(scrollY, 0.0f, maxScroll);

    const auto scrollbarWidth = hasScrollbar ? style.scrollbarWidth : 0.0f;
    pushClipRect(F32Rect2D{viewRect.x, viewRect.y, std::max(0.0f, viewRect.width - scrollbarWidth), viewRect.height});

    const auto top = viewRect.y + padding.y - scrollY;
    const auto left = viewRect.x + padding.x;
    context_.layoutStack_.push_back(detail::LayoutFrame{
        .left = left,
        .right = viewRect.x + viewRect.width - scrollbarWidth - padding.x,
        .top = top,
        .cursorX = left,
        .cursorY = top,
        .maxY = top,
        .rowHeight = 0.0f,
        .lastItemX = left,
        .requestedRight = left,
        .spacing = style.itemSpacing,
        .horizontal = false,
        .hasItems = false,
    });
    context_.scrollStack_.push_back(detail::ScrollFrame{
        .id = id,
        .viewRect = viewRect,
        .scrollY = scrollY,
        .maxScroll = maxScroll,
        .lastContentHeight = lastContentHeight,
        .paddingX = padding.x,
        .paddingY = padding.y,
        .requestedScrollY = 0.0f,
        .hasRequestedScroll = false,
        .hasScrollbar = hasScrollbar,
    });
}

void UI::scrollIntoView(const F32Rect2D& rect)
{
    POMDOG_ASSERT(!context_.scrollStack_.empty());
    if (context_.scrollStack_.empty()) {
        return;
    }
    auto& scroll = context_.scrollStack_.back();
    const auto& view = scroll.viewRect;

    // NOTE: rect is already offset by this frame's scroll, so the distance it
    // sticks out of the padded view is the change of the scroll offset.
    const auto top = view.y + scroll.paddingY;
    const auto bottom = view.y + view.height - scroll.paddingY;
    auto target = scroll.scrollY;
    if (rect.y < top) {
        target -= top - rect.y;
    }
    else if (rect.y + rect.height > bottom) {
        target += std::min(rect.y + rect.height - bottom, rect.y - top);
    }
    if (target != scroll.scrollY) {
        scroll.requestedScrollY = target;
        scroll.hasRequestedScroll = true;
        requestUpdateAt(input_.timeSeconds);
    }
}

ScrollResult UI::endScrollRegion()
{
    POMDOG_ASSERT(!context_.scrollStack_.empty());
    POMDOG_ASSERT(context_.layoutStack_.size() > 1);

    const auto frame = context_.layoutStack_.back();
    context_.layoutStack_.pop_back();
    const auto scroll = context_.scrollStack_.back();
    context_.scrollStack_.pop_back();
    popClipRect();

    ScrollResult result;
    result.contentHeight = (frame.maxY - frame.top) + scroll.paddingY * 2.0f;
    result.scrollY = scroll.scrollY;
    if (scroll.hasRequestedScroll) {
        result.scrollY = std::clamp(scroll.requestedScrollY, 0.0f, std::max(0.0f, result.contentHeight - scroll.viewRect.height));
    }

    // NOTE: The scroll bar takes width from the content, so count it.
    const auto contentLeft = scroll.viewRect.x + scroll.paddingX;
    result.contentWidth = std::max(0.0f, frame.requestedRight - contentLeft) + scroll.paddingX * 2.0f;
    if (scroll.hasScrollbar) {
        result.contentWidth += context_.style_.scrollbarWidth;
    }

    if (result.contentHeight != scroll.lastContentHeight) {
        // NOTE: This frame was laid out with the previous content height, which
        // decides the scroll bar and the scroll range. Ask for one more frame so
        // the region settles even when no input arrives.
        requestUpdateAt(input_.timeSeconds);
    }

    if (!scroll.hasScrollbar) {
        return result;
    }

    const auto& style = context_.style_;
    const auto& view = scroll.viewRect;
    const auto track = F32Rect2D{view.x + view.width - style.scrollbarWidth, view.y, style.scrollbarWidth, view.height};
    const auto thumbLength = std::clamp(view.height * view.height / scroll.lastContentHeight, std::min(minThumbLength, track.height), track.height);
    const auto thumbRange = track.height - thumbLength;
    auto thumbOffset = 0.0f;
    if ((scroll.maxScroll > 0.0f) && (thumbRange > 0.0f)) {
        thumbOffset = scroll.scrollY / scroll.maxScroll * thumbRange;
    }

    const auto response = interact(combineID(scroll.id, scrollbarKey), track);

    // NOTE: A thin thumb that widens while the pointer is on the scroll bar.
    const auto inset = (response.hovered || response.active) ? 2.0f : 3.0f;
    const auto thumb = F32Rect2D{track.x + inset, track.y + thumbOffset + 2.0f, track.width - inset * 2.0f, std::max(0.0f, thumbLength - 4.0f)};

    if ((thumbRange > 0.0f) && response.isHeld()) {
        if (response.activated && !thumb.contains(input_.pointerPosition)) {
            // NOTE: A press on the track outside the thumb centers the thumb on the pointer.
            const auto t = (input_.pointerPosition.y - track.y - thumbLength * 0.5f) / thumbRange;
            result.scrollY = t * scroll.maxScroll;
        }
        else if (!response.activated) {
            result.scrollY += pointerDelta_.y * scroll.maxScroll / thumbRange;
        }
        result.scrollY = std::clamp(result.scrollY, 0.0f, scroll.maxScroll);
    }
    if (result.scrollY != scroll.scrollY) {
        // NOTE: The content and thumb were drawn with the previous offset.
        requestUpdateAt(input_.timeSeconds);
    }

    auto thumbColor = style.scrollbarThumbColor;
    if (response.active) {
        thumbColor = style.scrollbarThumbActiveColor;
    }
    else if (response.hovered) {
        thumbColor = style.scrollbarThumbHoveredColor;
    }
    fillRect(thumb, thumbColor, thumb.width * 0.5f);
    return result;
}

void UI::requestUpdateAt(f64 timeSeconds) noexcept
{
    if (!wakeTime_ || (timeSeconds < *wakeTime_)) {
        wakeTime_ = timeSeconds;
    }
}

const Input& UI::getInput() const noexcept
{
    return input_;
}

const Style& UI::getStyle() const noexcept
{
    return context_.style_;
}

const F32Rect2D& UI::getClipRect() const noexcept
{
    POMDOG_ASSERT(!context_.clipStack_.empty());
    const auto clipRects = context_.drawList_.getClipRects();
    return clipRects[context_.clipStack_.back()];
}

bool UI::isPointerOverCurrentLayer() const noexcept
{
    return input_.pointerPresent &&
           (currentWindow_ == hoveredWindow_) &&
           getClipRect().contains(input_.pointerPosition);
}

UI::LayerState UI::enterLayer(WidgetID layer, u64 depth)
{
    const auto saved = LayerState{
        .window = currentWindow_,
        .windowIndex = currentWindowIndex_,
        .depth = currentDepth_,
    };
    currentWindow_ = layer;
    currentWindowIndex_ = -1;
    currentDepth_ = depth;
    context_.drawList_.beginRange(depth);

    // NOTE: A layer above the windows is clipped only by the viewport.
    context_.clipStack_.push_back(context_.clipStack_.front());
    return saved;
}

void UI::leaveLayer(const LayerState& saved)
{
    POMDOG_ASSERT(context_.clipStack_.size() > 1);
    context_.clipStack_.pop_back();
    currentWindow_ = saved.window;
    currentWindowIndex_ = saved.windowIndex;
    currentDepth_ = saved.depth;
    context_.drawList_.beginRange(saved.depth);
}

void UI::updateKeyRepeat()
{
    const auto down = input_.editKeysDown;
    keysPressed_ = down & ~context_.lastInput_.editKeysDown;

    // NOTE: Keys repeat only for a widget with keyboard focus, so holding keys
    // for the game does not keep waking the UI.
    if (context_.focusedID_.value == 0) {
        context_.repeatKeys_ = 0;
        return;
    }

    const auto now = input_.timeSeconds;
    if ((keysPressed_ & repeatableKeys) != 0) {
        context_.repeatKeys_ = keysPressed_ & repeatableKeys;
        context_.repeatTime_ = now + keyRepeatDelay;
    }
    else if (((context_.repeatKeys_ & down) != 0) && (now >= context_.repeatTime_)) {
        keysPressed_ |= (context_.repeatKeys_ & down);
        context_.repeatTime_ = std::max(context_.repeatTime_ + keyRepeatInterval, now);
    }

    if ((context_.repeatKeys_ & down) == 0) {
        context_.repeatKeys_ = 0;
    }
    if (context_.repeatKeys_ != 0) {
        requestUpdateAt(context_.repeatTime_);
    }
}

void UI::addCommand(const DrawCommand& command, std::string_view text)
{
    auto withClip = command;
    withClip.clipIndex = context_.clipStack_.back();
    context_.drawList_.addCommand(withClip, text);
}

} // namespace pomdog::devgui
