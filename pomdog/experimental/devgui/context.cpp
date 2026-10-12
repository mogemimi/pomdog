// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/context.h"
#include "pomdog/experimental/devgui/text_measurer.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui::detail {

void WidgetIDSet::clear() noexcept
{
    std::fill(buckets_.begin(), buckets_.end(), u64(0));
    count_ = 0;
}

bool WidgetIDSet::insert(WidgetID id)
{
    if ((count_ + 1) * 2 > buckets_.size()) {
        grow();
    }

    const auto mask = static_cast<u32>(buckets_.size() - 1);
    auto bucket = static_cast<u32>(id.value) & mask;
    while (buckets_[bucket] != 0) {
        if (buckets_[bucket] == id.value) {
            return false;
        }
        bucket = (bucket + 1) & mask;
    }
    buckets_[bucket] = id.value;
    ++count_;
    return true;
}

void WidgetIDSet::grow()
{
    std::vector<u64> old;
    old.swap(buckets_);
    buckets_.assign(std::max<std::size_t>(old.size() * 2, 256), u64(0));
    count_ = 0;
    for (const auto id : old) {
        if (id != 0) {
            [[maybe_unused]] const auto inserted = insert(WidgetID{id});
            POMDOG_ASSERT(inserted);
        }
    }
}

} // namespace pomdog::devgui::detail

namespace pomdog::devgui {

TextMeasurer::~TextMeasurer() = default;

Context::Context(std::shared_ptr<TextMeasurer> textMeasurer, u32 stateCapacity)
    : textMeasurer_(std::move(textMeasurer))
{
    POMDOG_ASSERT(textMeasurer_ != nullptr);
    states_.initialize(stateCapacity);
}

Context::~Context() = default;

bool Context::needsUpdate(const Input& input) const noexcept
{
    POMDOG_ASSERT(!frameActive_);
    if (invalidated_ || (frameCount_ == 0)) {
        return true;
    }
    if (input.viewportSize != lastInput_.viewportSize) {
        return true;
    }
    if (wakeTime_ && (input.timeSeconds >= *wakeTime_)) {
        return true;
    }

    const bool pointerChanged = (input.pointerPresent != lastInput_.pointerPresent) ||
                                (input.pointerPosition != lastInput_.pointerPosition);
    const bool wheelMoved = (input.wheelDelta.x != 0.0f) || (input.wheelDelta.y != 0.0f);

    // NOTE: Even an outside press must build a frame to record its history.
    // Otherwise entering a window or focusing a field while held can synthesize
    // a new press. The query itself never consumes these transitions.
    if ((input.primaryButtonDown != lastInput_.primaryButtonDown) ||
        (input.secondaryButtonDown != lastInput_.secondaryButtonDown) ||
        (input.editKeysDown != lastInput_.editKeysDown)) {
        return true;
    }

    if (focusedID_.value != 0) {
        // NOTE: A focused text field reacts to typing. Held keys repeat through
        // the deadline from `UI::requestUpdateAt()`.
        if (!input.textInput.empty()) {
            return true;
        }
    }

    if (activeID_.value != 0) {
        // NOTE: An active widget follows the pointer wherever it goes.
        return pointerChanged || wheelMoved || (input.shiftDown != lastInput_.shiftDown);
    }

    const bool overUI = input.pointerPresent && isPointerOverUI(input.pointerPosition);
    const bool wasOverUI = (hoveredWindow_.value != 0) || (hoveredID_.value != 0);

    if (pointerChanged && (overUI || wasOverUI)) {
        return true;
    }
    if (wheelMoved && overUI) {
        return true;
    }
    return false;
}

void Context::invalidate() noexcept
{
    invalidated_ = true;
}

const DrawList& Context::getDrawList() const noexcept
{
    return drawList_;
}

InputCapture Context::getInputCapture() const noexcept
{
    return capture_;
}

const Style& Context::getStyle() const noexcept
{
    return style_;
}

void Context::setStyle(const Style& style) noexcept
{
    style_ = style;
    invalidated_ = true;
}

u64 Context::getFrameCount() const noexcept
{
    return frameCount_;
}

u32 Context::getDuplicateIDCount() const noexcept
{
    return duplicateIDCount_;
}

bool Context::isPointerOverUI(const Vector2& position) const noexcept
{
    for (const auto& popup : popups_) {
        if (popup.rect.contains(position)) {
            return true;
        }
    }
    for (const auto& dockSpace : dockSpaces_) {
        if ((dockSpace.lastFrame == frameCount_) && dockSpace.rect.contains(position)) {
            return true;
        }
    }
    for (const auto& window : windows_) {
        if ((window.lastFrame == frameCount_) && !window.docked && getWindowHitRect(window).contains(position)) {
            return true;
        }
    }
    for (const auto& rect : backgroundHotRects_) {
        if (rect.contains(position)) {
            return true;
        }
    }
    return false;
}

F32Rect2D Context::getWindowHitRect(const detail::WindowState& window) const noexcept
{
    if (window.collapsed) {
        return F32Rect2D{window.rect.x, window.rect.y, window.rect.width, style_.titleBarHeight};
    }
    return window.rect;
}

} // namespace pomdog::devgui
