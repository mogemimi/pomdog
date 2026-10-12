// Copyright mogemimi. Distributed under the MIT license.

#include "tests/experimental/devgui/devgui_test_harness.h"
#include "tests/testing/testing.h"
#include "pomdog/experimental/devgui/devgui.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <algorithm>
#include <array>
#include <iterator>
#include <optional>
#include <ostream>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace devgui = pomdog::devgui;
using pomdog::f32;
using pomdog::F32Rect2D;
using pomdog::i32;
using pomdog::u32;
using namespace pomdog::devgui::literals;

using devgui_test::drawsText;
using devgui_test::Harness;

// NOTE: See devgui_test_harness.h for the positions these tests rely on. Rows
// outside any window span x = 8 to 792 and are 20 pixels tall with no space
// between them, so they span y = 6 to 26, 26 to 46, and 46 to 66. Each row
// keeps 20 pixels at its right end, where a close button shows while the row
// is hovered.

namespace {

constexpr std::array<const char*, 3> rowNames = {{"First", "Second", "Third"}};

/// Returns how many times the draw list draws text exactly equal to text.
[[nodiscard]] i32
countText(const devgui::DrawList& drawList, const char* text)
{
    i32 count = 0;
    for (const auto& command : drawList.getCommands()) {
        if ((command.kind == devgui::DrawCommandKind::Text) && (drawList.getText(command) == std::string_view{text})) {
            ++count;
        }
    }
    return count;
}

/// Returns true when the draw list fills a rectangle with color.
[[nodiscard]] bool
fillsWith(const devgui::DrawList& drawList, const pomdog::Color& color)
{
    const auto commands = drawList.getCommands();
    return std::any_of(commands.begin(), commands.end(), [&](const devgui::DrawCommand& command) {
        return (command.kind == devgui::DrawCommandKind::FillRect) && (command.color == color);
    });
}

} // namespace

TEST_CASE("devgui::listView")
{
    Harness h;
    std::array<u32, 3> items = {{0, 1, 2}};
    std::array<devgui::Response, 3> rows = {};
    std::array<bool, 3> closed = {{false, false, false}};
    std::optional<devgui::ListMove> move;
    bool reorderable = true;

    const auto build = [&](devgui::UI& ui) {
        auto list = devgui::listView(ui, "list"_key, devgui::ListViewOptions{.reorderable = reorderable});
        for (std::size_t i = 0; i < items.size(); ++i) {
            const auto item = items[i];
            const auto options = devgui::ListItemOptions{.selected = (item == 2), .trailingWidth = 20.0f};
            const auto row = list.item("row"_key.at(item), rowNames[item], options);
            rows[item] = row;
            if (row.hovered) {
                const auto closeRect = F32Rect2D{row.rect.x + row.rect.width - 20.0f, row.rect.y, 20.0f, row.rect.height};
                if (devgui::closeButton(ui, "close"_key.at(item), closeRect).clicked) {
                    closed[item] = true;
                }
            }
        }
        move = list.getMove();
        if (move) {
            const auto from = std::next(items.begin(), move->from);
            const auto to = std::next(items.begin(), move->to);
            if (move->from < move->to) {
                std::rotate(from, std::next(from), std::next(to));
            }
            else {
                std::rotate(to, from, std::next(from));
            }
        }
    };
    const auto dropColor = h.context.getStyle().dropIndicatorColor;

    h.frame(build);
    REQUIRE(rows[0].rect.y == 6.0f);
    REQUIRE(rows[1].rect.y == 26.0f);
    REQUIRE(rows[2].rect.y == 46.0f);
    REQUIRE(drawsText(h.context.getDrawList(), "Second"));

    SUBCASE("moves a dragged row to where it is dropped")
    {
        h.moveTo(100.0f, 16.0f);
        h.frame(build);
        h.input.primaryButtonDown = true;
        h.frame(build);
        REQUIRE(rows[0].activated);

        // NOTE: While dragged, the row stays in place, a copy follows the
        // pointer, and a line shows the edge below the middle row.
        h.moveTo(100.0f, 50.0f);
        h.frame(build);
        REQUIRE_FALSE(move);
        REQUIRE(rows[0].rect.y == 6.0f);
        REQUIRE(countText(h.context.getDrawList(), "First") == 2);
        REQUIRE(fillsWith(h.context.getDrawList(), dropColor));

        h.input.primaryButtonDown = false;
        h.frame(build);
        REQUIRE(move);
        REQUIRE(move->from == 0);
        REQUIRE(move->to == 1);
        REQUIRE(items == std::array<u32, 3>{{1, 0, 2}});
        REQUIRE(h.context.needsUpdate(h.input));

        h.frame(build);
        REQUIRE(rows[0].rect.y == 26.0f);
        REQUIRE(countText(h.context.getDrawList(), "First") == 1);
        REQUIRE_FALSE(fillsWith(h.context.getDrawList(), dropColor));
    }
    SUBCASE("moves a row up past several rows")
    {
        h.moveTo(100.0f, 56.0f);
        h.frame(build);
        h.input.primaryButtonDown = true;
        h.frame(build);
        h.moveTo(100.0f, 8.0f);
        h.frame(build);
        h.input.primaryButtonDown = false;
        h.frame(build);
        REQUIRE(move);
        REQUIRE(move->from == 2);
        REQUIRE(move->to == 0);
        REQUIRE(items == std::array<u32, 3>{{2, 0, 1}});
    }
    SUBCASE("reports no move for a drop next to the row itself")
    {
        h.moveTo(100.0f, 30.0f);
        h.frame(build);
        h.input.primaryButtonDown = true;
        h.frame(build);
        h.moveTo(100.0f, 40.0f);
        h.frame(build);
        h.input.primaryButtonDown = false;
        h.frame(build);
        REQUIRE_FALSE(move);
        REQUIRE(items == std::array<u32, 3>{{0, 1, 2}});
    }
    SUBCASE("does not drag a row that is only clicked")
    {
        h.click(100.0f, 16.0f, build);
        REQUIRE(rows[0].clicked);
        REQUIRE_FALSE(move);
        REQUIRE(countText(h.context.getDrawList(), "First") == 1);
    }
    SUBCASE("keeps the order when reordering is off")
    {
        reorderable = false;
        h.moveTo(100.0f, 16.0f);
        h.frame(build);
        h.input.primaryButtonDown = true;
        h.frame(build);
        h.moveTo(100.0f, 70.0f);
        h.frame(build);
        REQUIRE_FALSE(fillsWith(h.context.getDrawList(), dropColor));
        h.input.primaryButtonDown = false;
        h.frame(build);
        REQUIRE_FALSE(move);
        REQUIRE(items == std::array<u32, 3>{{0, 1, 2}});
    }
    SUBCASE("leaves the press to a close button over the row")
    {
        h.click(780.0f, 36.0f, build);
        REQUIRE(closed[1]);
        REQUIRE_FALSE(rows[1].clicked);
        REQUIRE(rows[1].hovered);
    }
    SUBCASE("takes a press on the rest of the row")
    {
        h.click(100.0f, 36.0f, build);
        REQUIRE(rows[1].clicked);
        REQUIRE_FALSE(closed[1]);
    }
}
