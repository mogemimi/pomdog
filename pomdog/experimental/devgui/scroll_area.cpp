// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/scroll_area.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/widget_helpers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <bit>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
namespace {

// NOTE: A scroll area keeps its offset and content height in one stored value.
[[nodiscard]] u64
packScrollState(const ScrollResult& result) noexcept
{
    return (static_cast<u64>(std::bit_cast<u32>(result.contentHeight)) << 32) |
           static_cast<u64>(std::bit_cast<u32>(result.scrollY));
}

[[nodiscard]] ScrollResult
unpackScrollState(u64 value) noexcept
{
    ScrollResult result;
    result.scrollY = std::bit_cast<f32>(static_cast<u32>(value & 0xFFFFFFFFULL));
    result.contentHeight = std::bit_cast<f32>(static_cast<u32>(value >> 32));
    return result;
}

} // namespace

ScrollAreaScope::ScrollAreaScope(UI& ui, Key key, f32 height)
    : ui_(ui)
    , id_(ui.makeID(key))
{
    const auto& style = ui_.getStyle();

    ScrollResult state;
    if (const auto stored = ui_.loadState(id_); stored) {
        state = unpackScrollState(*stored);
    }

    const auto rect = ui_.allocate(0.0f, height);
    detail::drawFrame(ui_, rect, style.titleBarColor, style.inputBorderColor, style.frameRounding);
    ui_.pushIDScope(key);
    ui_.beginScrollRegion(id_, rect, state.scrollY, state.contentHeight, style.framePadding);
}

ScrollAreaScope::~ScrollAreaScope()
{
    const auto result = ui_.endScrollRegion();
    ui_.popIDScope();
    ui_.storeState(id_, packScrollState(result));
}

ScrollAreaScope scrollArea(UI& ui, Key key, f32 height)
{
    return ScrollAreaScope{ui, key, height};
}

} // namespace pomdog::devgui
