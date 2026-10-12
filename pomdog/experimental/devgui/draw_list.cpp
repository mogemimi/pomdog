// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/draw_list.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {

DrawList::DrawList() noexcept = default;

void DrawList::reset(const Vector2& viewportSize) noexcept
{
    commands_.clear();
    clipRects_.clear();
    ranges_.clear();
    gradients_.clear();
    text_.clear();
    viewportSize_ = viewportSize;
}

u16 DrawList::addClipRect(const F32Rect2D& rect)
{
    POMDOG_ASSERT(clipRects_.size() < std::numeric_limits<u16>::max());
    clipRects_.push_back(rect);
    return static_cast<u16>(clipRects_.size() - 1);
}

void DrawList::addCommand(const DrawCommand& command, std::string_view text)
{
    POMDOG_ASSERT(!ranges_.empty());
    auto& added = commands_.emplace_back(command);
    if (command.kind == DrawCommandKind::Text) {
        added.textOffset = static_cast<u32>(text_.size());
        added.textLength = static_cast<u32>(text.size());
        text_.append(text);
    }
}

void DrawList::addCommand(const DrawCommand& command, const RectGradient& gradient)
{
    POMDOG_ASSERT(!ranges_.empty());
    POMDOG_ASSERT(command.kind == DrawCommandKind::FillRectGradient);
    auto& added = commands_.emplace_back(command);
    added.gradientIndex = static_cast<u32>(gradients_.size());
    gradients_.push_back(gradient);
}

void DrawList::beginRange(u64 depth)
{
    const auto position = static_cast<u32>(commands_.size());
    if (!ranges_.empty()) {
        auto& last = ranges_.back();
        last.commandEnd = position;
        if (last.commandBegin == last.commandEnd) {
            // NOTE: Reuse the empty range instead of keeping it.
            last.depth = depth;
            return;
        }
    }
    ranges_.push_back(DrawRange{position, position, depth});
}

void DrawList::finish() noexcept
{
    if (ranges_.empty()) {
        return;
    }
    ranges_.back().commandEnd = static_cast<u32>(commands_.size());
    if (ranges_.back().commandBegin == ranges_.back().commandEnd) {
        ranges_.pop_back();
    }

    // NOTE: An insertion sort is stable and needs no temporary buffer, unlike
    // std::stable_sort. A frame has a few ranges per window.
    for (std::size_t i = 1; i < ranges_.size(); ++i) {
        auto range = ranges_[i];
        auto j = i;
        while ((j > 0) && (ranges_[j - 1].depth > range.depth)) {
            ranges_[j] = ranges_[j - 1];
            --j;
        }
        ranges_[j] = range;
    }
}

Vector2 DrawList::getViewportSize() const noexcept
{
    return viewportSize_;
}

std::span<const DrawCommand> DrawList::getCommands() const noexcept
{
    return commands_;
}

std::span<const F32Rect2D> DrawList::getClipRects() const noexcept
{
    return clipRects_;
}

std::span<const DrawRange> DrawList::getRanges() const noexcept
{
    return ranges_;
}

std::string_view DrawList::getText(const DrawCommand& command) const noexcept
{
    POMDOG_ASSERT(command.kind == DrawCommandKind::Text);
    POMDOG_ASSERT(static_cast<std::size_t>(command.textOffset) + command.textLength <= text_.size());
    return std::string_view{text_}.substr(command.textOffset, command.textLength);
}

const RectGradient& DrawList::getGradient(const DrawCommand& command) const noexcept
{
    POMDOG_ASSERT(command.kind == DrawCommandKind::FillRectGradient);
    POMDOG_ASSERT(command.gradientIndex < gradients_.size());
    return gradients_[command.gradientIndex];
}

} // namespace pomdog::devgui
