// Copyright mogemimi. Distributed under the MIT license.

#include "tests/experimental/devgui/devgui_test_harness.h"
#include "tests/testing/testing.h"
#include "pomdog/experimental/devgui/devgui.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <limits>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace devgui = pomdog::devgui;
using pomdog::f32;
using pomdog::f64;
using pomdog::i32;
using pomdog::Vector2;
using namespace pomdog::devgui::literals;

using devgui_test::drawsText;
using devgui_test::Harness;
using devgui_test::windowAt;

// NOTE: See devgui_test_harness.h for the positions these tests rely on. A
// slider keeps 56 pixels for its value and 6 for spacing, so in a property row
// outside any window it takes x = 321 to 730, and its track runs 6 pixels
// inside that for the thumb.

namespace {

constexpr f32 trackLeft = 327.0f;
constexpr f32 trackWidth = 397.0f;

/// Returns the last window title drawn, which is the frontmost window's.
[[nodiscard]] std::string
frontTitle(const devgui::DrawList& drawList)
{
    std::string result;
    const auto commands = drawList.getCommands();
    for (const auto& range : drawList.getRanges()) {
        for (auto i = range.commandBegin; i < range.commandEnd; ++i) {
            if (commands[i].kind != devgui::DrawCommandKind::Text) {
                continue;
            }
            const auto text = drawList.getText(commands[i]);
            if ((text == "A") || (text == "B")) {
                result = text;
            }
        }
    }
    return result;
}

} // namespace

TEST_CASE("devgui::button")
{
    Harness h;
    devgui::Response r;
    const auto build = [&](devgui::UI& ui) { r = devgui::button(ui, "ok"_key, "OK"); };

    SUBCASE("is sized to its label")
    {
        h.frame(build);
        REQUIRE(r.rect.x == 8.0f);
        REQUIRE(r.rect.y == 6.0f);
        REQUIRE(r.rect.width == 26.0f);
        REQUIRE(r.rect.height == 20.0f);
    }
    SUBCASE("clicks when pressed and released on it")
    {
        h.moveTo(12.0f, 10.0f);
        h.frame(build);
        REQUIRE(r.hovered);
        REQUIRE_FALSE(r.active);

        h.input.primaryButtonDown = true;
        h.frame(build);
        REQUIRE(r.activated);
        REQUIRE(r.active);
        REQUIRE_FALSE(r.clicked);

        h.input.primaryButtonDown = false;
        h.frame(build);
        REQUIRE(r.deactivated);
        REQUIRE(r.clicked);
        REQUIRE_FALSE(r.active);
    }
    SUBCASE("does not click when released outside")
    {
        h.moveTo(12.0f, 10.0f);
        h.frame(build);
        h.input.primaryButtonDown = true;
        h.frame(build);
        h.moveTo(300.0f, 300.0f);
        h.frame(build);
        REQUIRE(r.active);
        REQUIRE_FALSE(r.hovered);

        h.input.primaryButtonDown = false;
        h.frame(build);
        REQUIRE(r.deactivated);
        REQUIRE_FALSE(r.clicked);
    }
    SUBCASE("ignores a press that started elsewhere")
    {
        h.input.primaryButtonDown = true;
        h.frame(build);
        h.moveTo(12.0f, 10.0f);
        h.frame(build);
        REQUIRE_FALSE(r.active);

        h.input.primaryButtonDown = false;
        h.frame(build);
        REQUIRE_FALSE(r.clicked);
    }
}

TEST_CASE("devgui::checkbox")
{
    Harness h;
    bool value = false;
    devgui::Response r;
    const auto build = [&](devgui::UI& ui) { r = devgui::checkbox(ui, "enabled"_key, "Enabled", value); };

    h.moveTo(12.0f, 10.0f);
    h.frame(build);
    h.input.primaryButtonDown = true;
    h.frame(build);
    REQUIRE_FALSE(value);

    h.input.primaryButtonDown = false;
    h.frame(build);
    REQUIRE(value);
    REQUIRE(r.changed);

    h.frame(build);
    REQUIRE(value);
    REQUIRE_FALSE(r.changed);

    SUBCASE("draws the check as one connected stroke")
    {
        pomdog::u32 strokes = 0;
        for (const auto& command : h.context.getDrawList().getCommands()) {
            REQUIRE(command.kind != devgui::DrawCommandKind::Line);
            if (command.kind == devgui::DrawCommandKind::Polyline) {
                ++strokes;
                REQUIRE(command.p0.x < command.p1.x);
                REQUIRE(command.p1.x < command.p2.x);
                REQUIRE(command.p0.y < command.p1.y);
                REQUIRE(command.p2.y < command.p1.y);
            }
        }
        REQUIRE(strokes == 1);
    }
}

TEST_CASE("devgui::sliderFloat")
{
    Harness h;
    f32 value = 0.0f;
    devgui::Response r;
    const auto build = [&](devgui::UI& ui) { r = devgui::sliderFloat(ui, "speed"_key, "Speed", value, 0.0f, 10.0f); };

    h.moveTo(trackLeft + trackWidth * 0.25f, 16.0f);
    h.frame(build);
    REQUIRE(r.rect.x == 321.0f);
    REQUIRE(r.rect.width == 409.0f);
    REQUIRE(value == 0.0f);

    h.input.primaryButtonDown = true;
    h.frame(build);
    REQUIRE(r.activated);
    REQUIRE(r.changed);
    REQUIRE(value == doctest::Approx(2.5));

    h.moveTo(900.0f, 16.0f);
    h.frame(build);
    REQUIRE(r.changed);
    REQUIRE(value == doctest::Approx(10.0));

    // NOTE: The pointer movement on the release frame still counts.
    h.moveTo(trackLeft + trackWidth * 0.5f, 16.0f);
    h.input.primaryButtonDown = false;
    h.frame(build);
    REQUIRE(r.deactivated);
    REQUIRE(r.changed);
    REQUIRE(value == doctest::Approx(5.0));

    h.frame(build);
    REQUIRE_FALSE(r.changed);
    REQUIRE(value == doctest::Approx(5.0));
}

TEST_CASE("devgui::sliderInt")
{
    Harness h;
    i32 value = 0;
    i32 minValue = 0;
    i32 maxValue = 4;
    const auto build = [&](devgui::UI& ui) { devgui::sliderInt(ui, "count"_key, "Count", value, minValue, maxValue); };

    SUBCASE("rounds to the nearest integer")
    {
        // NOTE: 0.6 of the way across rounds to 2.4 and then to 2.
        h.moveTo(trackLeft + trackWidth * 0.6f, 16.0f);
        h.frame(build);
        h.input.primaryButtonDown = true;
        h.frame(build);
        REQUIRE(value == 2);
    }
    SUBCASE("reaches both ends of the full integer range")
    {
        minValue = std::numeric_limits<i32>::min();
        maxValue = std::numeric_limits<i32>::max();
        h.moveTo(trackLeft + trackWidth, 16.0f);
        h.frame(build);
        h.input.primaryButtonDown = true;
        h.frame(build);
        REQUIRE(value == maxValue);

        h.moveTo(trackLeft, 16.0f);
        h.frame(build);
        REQUIRE(value == minValue);
    }
    SUBCASE("keeps adjacent values distinct near the maximum integer")
    {
        maxValue = std::numeric_limits<i32>::max();
        minValue = maxValue - 4;
        h.moveTo(trackLeft + trackWidth * 0.5f, 16.0f);
        h.frame(build);
        h.input.primaryButtonDown = true;
        h.frame(build);
        REQUIRE(value == maxValue - 2);
    }
}

TEST_CASE("devgui::dragInt")
{
    Harness h;
    i32 value = 10;
    devgui::Response r;
    const auto build = [&](devgui::UI& ui) { r = devgui::dragInt(ui, "count"_key, "Count", value, 0.25f, 0, 11); };

    h.moveTo(400.0f, 16.0f);
    h.frame(build);
    h.input.primaryButtonDown = true;
    h.frame(build);
    REQUIRE(r.activated);
    REQUIRE(value == 10);

    // NOTE: Each 2-pixel move adds half a step, so the value moves every
    // second frame.
    h.moveTo(402.0f, 16.0f);
    h.frame(build);
    REQUIRE(value == 10);
    h.moveTo(404.0f, 16.0f);
    h.frame(build);
    REQUIRE(value == 11);
    REQUIRE(r.changed);

    SUBCASE("stops at the maximum")
    {
        h.moveTo(500.0f, 16.0f);
        h.frame(build);
        REQUIRE(value == 11);
    }
    SUBCASE("moves in finer steps with shift")
    {
        h.input.shiftDown = true;
        h.moveTo(364.0f, 16.0f);
        h.frame(build);
        REQUIRE(value == 10);
    }
}

TEST_CASE("devgui::treeNode")
{
    Harness h;
    bool open = false;
    devgui::Response child;
    const auto build = [&](devgui::UI& ui) {
        if (auto node = devgui::treeNode(ui, "node"_key, "Node"); node.isOpen()) {
            open = true;
            child = devgui::button(ui, "child"_key, "Child");
        }
        else {
            open = false;
        }
    };

    SUBCASE("opens on click and stays open")
    {
        h.moveTo(12.0f, 10.0f);
        h.frame(build);
        REQUIRE_FALSE(open);

        h.input.primaryButtonDown = true;
        h.frame(build);
        h.input.primaryButtonDown = false;
        h.frame(build);
        REQUIRE(open);

        h.moveTo(500.0f, 500.0f);
        h.frame(build);
        REQUIRE(open);
        REQUIRE(child.rect.x == 8.0f + 14.0f);
    }
    SUBCASE("can start open")
    {
        h.frame([&](devgui::UI& ui) {
            auto node = devgui::treeNode(ui, "node"_key, "Node", devgui::TreeNodeOptions{.defaultOpen = true});
            open = node.isOpen();
        });
        REQUIRE(open);
    }
    SUBCASE("lets sibling nodes reuse child keys")
    {
        h.frame([&](devgui::UI& ui) {
            for (pomdog::u32 i = 0; i < 3; ++i) {
                auto node = devgui::treeNode(ui, "emitter"_key.at(i), "Emitter", devgui::TreeNodeOptions{.defaultOpen = true});
                [[maybe_unused]] const auto clicked = devgui::button(ui, "remove"_key, "Remove");
            }
        });
        REQUIRE(h.context.getDuplicateIDCount() == 0);
    }
    SUBCASE("draws its arrow as one connected stroke in both directions")
    {
        const auto checkArrow = [&](bool expanded) {
            h.frame([&](devgui::UI& ui) {
                auto node = devgui::treeNode(ui, "node"_key, "Node", devgui::TreeNodeOptions{.defaultOpen = expanded});
            });
            const auto commands = h.context.getDrawList().getCommands();
            REQUIRE(commands.size() == 2);
            REQUIRE(commands[0].kind == devgui::DrawCommandKind::Polyline);
            if (expanded) {
                REQUIRE(commands[0].p0.y < commands[0].p1.y);
                REQUIRE(commands[0].p2.y < commands[0].p1.y);
            }
            else {
                REQUIRE(commands[0].p0.x < commands[0].p1.x);
                REQUIRE(commands[0].p2.x < commands[0].p1.x);
            }
        };
        checkArrow(false);
        checkArrow(true);
    }
}

TEST_CASE("devgui duplicate IDs")
{
    Harness h;
    devgui::Response first;
    devgui::Response second;
    const auto build = [&](devgui::UI& ui) {
        auto row = devgui::horizontal(ui);
        first = devgui::button(ui, "same"_key, "A");
        second = devgui::button(ui, "same"_key, "B");
    };

    // NOTE: Each button is 19 pixels wide, so the second one starts at
    // x = 8 + 19 + 6 = 33.
    h.moveTo(35.0f, 10.0f);
    h.frame(build);
    REQUIRE(second.rect.x == 33.0f);
    REQUIRE(h.context.getDuplicateIDCount() == 1);
    REQUIRE_FALSE(second.hovered);

    SUBCASE("indices make keys unique")
    {
        h.frame([&](devgui::UI& ui) {
            for (pomdog::u32 i = 0; i < 4; ++i) {
                [[maybe_unused]] const auto clicked = devgui::button(ui, "item"_key.at(i), "Item");
            }
        });
        REQUIRE(h.context.getDuplicateIDCount() == 0);
    }
}

TEST_CASE("devgui::window")
{
    Harness h;

    SUBCASE("only the frontmost window takes the pointer")
    {
        f32 value = 0.0f;
        devgui::Response slider;
        const auto build = [&](devgui::UI& ui) {
            if (auto window = devgui::window(ui, "a"_key, "A", windowAt(0.0f, 0.0f, 300.0f, 300.0f)); window.isOpen()) {
                // NOTE: The control spans x = 121 to 292, partly under window B.
                slider = devgui::sliderFloat(ui, "value"_key, "Value", value, 0.0f, 1.0f);
            }
            if (auto window = devgui::window(ui, "b"_key, "B", windowAt(100.0f, 0.0f, 300.0f, 300.0f)); window.isOpen()) {
            }
        };

        h.frame(build);
        REQUIRE(frontTitle(h.context.getDrawList()) == "B");

        h.moveTo(150.0f, 36.0f);
        h.frame(build);
        REQUIRE_FALSE(slider.hovered);

        // NOTE: Pressing window A where B does not cover it brings A to the front.
        h.moveTo(50.0f, 36.0f);
        h.frame(build);
        h.input.primaryButtonDown = true;
        h.frame(build);
        h.input.primaryButtonDown = false;
        h.frame(build);
        REQUIRE(frontTitle(h.context.getDrawList()) == "A");

        h.moveTo(150.0f, 36.0f);
        h.frame(build);
        REQUIRE(slider.hovered);
    }
    SUBCASE("moves when its title bar is dragged")
    {
        devgui::Response button;
        const auto build = [&](devgui::UI& ui) {
            if (auto window = devgui::window(ui, "a"_key, "A", windowAt(100.0f, 100.0f, 200.0f, 150.0f)); window.isOpen()) {
                button = devgui::button(ui, "ok"_key, "OK");
            }
        };

        h.frame(build);
        REQUIRE(button.rect.x == 108.0f);
        REQUIRE(button.rect.y == 126.0f);

        h.moveTo(150.0f, 110.0f);
        h.frame(build);
        h.input.primaryButtonDown = true;
        h.frame(build);
        h.moveTo(170.0f, 130.0f);
        h.frame(build);
        h.input.primaryButtonDown = false;
        h.frame(build);
        REQUIRE(button.rect.x == 128.0f);
        REQUIRE(button.rect.y == 146.0f);
    }
    SUBCASE("collapses when its arrow is clicked")
    {
        bool open = false;
        const auto build = [&](devgui::UI& ui) {
            auto window = devgui::window(ui, "a"_key, "A", windowAt(100.0f, 100.0f, 200.0f, 150.0f));
            open = window.isOpen();
        };
        const auto checkArrow = [&] {
            pomdog::u32 strokes = 0;
            for (const auto& command : h.context.getDrawList().getCommands()) {
                if (command.kind == devgui::DrawCommandKind::Polyline) {
                    ++strokes;
                    if (open) {
                        REQUIRE(command.p0.y < command.p1.y);
                        REQUIRE(command.p2.y < command.p1.y);
                    }
                    else {
                        REQUIRE(command.p0.x < command.p1.x);
                        REQUIRE(command.p2.x < command.p1.x);
                    }
                }
            }
            REQUIRE(strokes == 1);
        };

        h.moveTo(110.0f, 110.0f);
        h.frame(build);
        REQUIRE(open);
        checkArrow();
        h.input.primaryButtonDown = true;
        h.frame(build);
        h.input.primaryButtonDown = false;
        h.frame(build);
        REQUIRE_FALSE(open);
        checkArrow();
        h.frame(build);
        REQUIRE_FALSE(open);
    }
}

TEST_CASE("devgui::scrollArea")
{
    // NOTE: The area spans y = 6 to 106. Twenty 20-pixel items with 4-pixel
    // spacing and 3-pixel padding make 482 pixels of content, so the area
    // scrolls up to 382 pixels.
    Harness h;
    devgui::Response firstItem;
    const auto build = [&](devgui::UI& ui) {
        auto area = devgui::scrollArea(ui, "list"_key, 100.0f);
        for (pomdog::u32 i = 0; i < 20; ++i) {
            const auto r = devgui::button(ui, "item"_key.at(i), "Item");
            if (i == 0) {
                firstItem = r;
            }
        }
    };

    h.moveTo(100.0f, 50.0f);
    h.frame(build);
    h.frame(build);
    REQUIRE(firstItem.rect.y == 9.0f);

    h.input.wheelDelta = Vector2{0.0f, -1.0f};
    h.frame(build);
    REQUIRE(firstItem.rect.y == 9.0f - 40.0f);
    REQUIRE(h.context.getInputCapture().wheel);

    h.input.wheelDelta = Vector2{0.0f, -100.0f};
    h.frame(build);
    REQUIRE(firstItem.rect.y == 9.0f - 382.0f);

    h.input.wheelDelta = Vector2{0.0f, 0.0f};
    h.frame(build);
    REQUIRE(firstItem.rect.y == 9.0f - 382.0f);
    REQUIRE_FALSE(h.context.getInputCapture().wheel);
}

TEST_CASE("devgui::UI::endScrollRegion")
{
    Harness h;
    f32 offset = 0.0f;
    f32 firstY = 0.0f;
    const auto build = [&](devgui::UI& ui) {
        ui.beginScrollRegion(ui.makeID("scroll"_key), pomdog::F32Rect2D{8.0f, 6.0f, 784.0f, 100.0f}, offset, 400.0f, Vector2{0.0f, 0.0f});
        firstY = ui.allocate(0.0f, 400.0f).y;
        offset = ui.endScrollRegion().scrollY;
    };

    SUBCASE("updates after a track press even while the pointer stays still")
    {
        h.moveTo(788.0f, 80.0f);
        h.frame(build);
        REQUIRE_FALSE(h.context.needsUpdate(h.input));
        h.input.primaryButtonDown = true;
        h.frame(build);
        REQUIRE(offset > 0.0f);
        REQUIRE(firstY == doctest::Approx(6.0f));
        REQUIRE(h.context.needsUpdate(h.input));

        h.frame(build);
        REQUIRE(firstY == doctest::Approx(6.0f - offset));
        REQUIRE_FALSE(h.context.needsUpdate(h.input));
    }
    SUBCASE("updates after pointer movement and release in the same frame")
    {
        h.moveTo(788.0f, 16.0f);
        h.frame(build);
        h.input.primaryButtonDown = true;
        h.frame(build);
        REQUIRE(offset == doctest::Approx(0.0f));
        REQUIRE_FALSE(h.context.needsUpdate(h.input));

        h.moveTo(788.0f, 66.0f);
        h.input.primaryButtonDown = false;
        h.frame(build);
        REQUIRE(offset > 0.0f);
        REQUIRE(firstY == doctest::Approx(6.0f));
        REQUIRE(h.context.needsUpdate(h.input));

        h.frame(build);
        REQUIRE(firstY == doctest::Approx(6.0f - offset));
        REQUIRE_FALSE(h.context.needsUpdate(h.input));
    }
}

TEST_CASE("devgui::Context::needsUpdate")
{
    Harness h;
    const auto& context = h.context;
    devgui::Response button;
    const auto build = [&](devgui::UI& ui) {
        if (auto window = devgui::window(ui, "a"_key, "A", windowAt(0.0f, 0.0f, 200.0f, 200.0f)); window.isOpen()) {
            button = devgui::button(ui, "ok"_key, "OK");
        }
    };

    REQUIRE(context.needsUpdate(h.input));
    h.frame(build);

    // NOTE: The first frame measures the window's content, so one more frame
    // follows to lay out with that height. Then the UI is idle.
    REQUIRE(h.context.needsUpdate(h.input));
    h.frame(build);
    REQUIRE_FALSE(h.context.needsUpdate(h.input));

    SUBCASE("ignores pointer movement and scrolling outside the UI")
    {
        h.moveTo(510.0f, 510.0f);
        REQUIRE_FALSE(h.context.needsUpdate(h.input));
        h.input.wheelDelta = Vector2{0.0f, 1.0f};
        REQUIRE_FALSE(h.context.needsUpdate(h.input));
    }
    SUBCASE("does not change input history when queried")
    {
        const auto position = h.input.pointerPosition;
        h.moveTo(510.0f, 510.0f);
        REQUIRE_FALSE(context.needsUpdate(h.input));
        REQUIRE_FALSE(context.needsUpdate(h.input));
        h.moveTo(525.0f, 535.0f);
        h.context.invalidate();
        REQUIRE(context.needsUpdate(h.input));
        REQUIRE(context.needsUpdate(h.input));
        auto delta = Vector2{0.0f, 0.0f};
        h.frame([&](devgui::UI& ui) {
            build(ui);
            delta = ui.getPointerDelta();
        });
        const auto expected = h.input.pointerPosition - position;
        REQUIRE(delta == expected);
    }
    SUBCASE("observes an outside press without turning entry into a click")
    {
        const auto x = button.rect.x + button.rect.width * 0.5f;
        const auto y = button.rect.y + button.rect.height * 0.5f;
        h.input.primaryButtonDown = true;
        REQUIRE(context.needsUpdate(h.input));
        REQUIRE(context.needsUpdate(h.input));
        h.frame(build);
        REQUIRE_FALSE(button.activated);
        REQUIRE_FALSE(context.needsUpdate(h.input));
        h.moveTo(x, y);
        REQUIRE(h.context.needsUpdate(h.input));
        // NOTE: Polling again must keep the same pending frame's input edges.
        REQUIRE(h.context.needsUpdate(h.input));
        h.frame(build);
        REQUIRE(button.hovered);
        REQUIRE_FALSE(button.activated);
        REQUIRE_FALSE(button.active);
        h.input.primaryButtonDown = false;
        REQUIRE(h.context.needsUpdate(h.input));
        h.frame(build);
        REQUIRE_FALSE(button.clicked);
        h.click(x, y, build);
        REQUIRE(button.clicked);
    }
    SUBCASE("records an outside release before the next click")
    {
        h.input.primaryButtonDown = true;
        REQUIRE(context.needsUpdate(h.input));
        h.frame(build);
        h.moveTo(510.0f, 510.0f);
        h.input.timeSeconds = 10.0;
        REQUIRE_FALSE(context.needsUpdate(h.input));
        h.input.primaryButtonDown = false;
        REQUIRE(context.needsUpdate(h.input));
        REQUIRE(context.needsUpdate(h.input));
        h.frame(build);
        REQUIRE_FALSE(context.needsUpdate(h.input));
        h.click(button.rect.x + button.rect.width * 0.5f, button.rect.y + button.rect.height * 0.5f, build);
        REQUIRE(button.clicked);
    }
    SUBCASE("records button transitions while the pointer is absent")
    {
        h.input.pointerPresent = false;
        h.input.primaryButtonDown = true;
        REQUIRE(context.needsUpdate(h.input));
        h.frame(build);
        REQUIRE_FALSE(button.activated);
        REQUIRE_FALSE(context.needsUpdate(h.input));
        h.input.primaryButtonDown = false;
        REQUIRE(context.needsUpdate(h.input));
        h.frame(build);
        REQUIRE_FALSE(button.clicked);
        REQUIRE_FALSE(context.needsUpdate(h.input));
    }
    SUBCASE("keeps a pending press across repeated update queries")
    {
        h.moveTo(button.rect.x + button.rect.width * 0.5f, button.rect.y + button.rect.height * 0.5f);
        REQUIRE(h.context.needsUpdate(h.input));
        h.frame(build);
        h.input.primaryButtonDown = true;
        REQUIRE(h.context.needsUpdate(h.input));
        REQUIRE(h.context.needsUpdate(h.input));
        h.frame(build);
        REQUIRE(button.activated);
        REQUIRE(button.active);
    }
    SUBCASE("updates when the pointer enters and leaves a window")
    {
        h.moveTo(50.0f, 50.0f);
        REQUIRE(h.context.needsUpdate(h.input));
        h.frame(build);
        REQUIRE(h.context.getInputCapture().pointer);

        h.moveTo(500.0f, 500.0f);
        REQUIRE(h.context.needsUpdate(h.input));
        h.frame(build);
        REQUIRE_FALSE(h.context.getInputCapture().pointer);
        REQUIRE_FALSE(h.context.needsUpdate(h.input));
    }
    SUBCASE("updates on invalidate and viewport changes")
    {
        h.context.invalidate();
        REQUIRE(h.context.needsUpdate(h.input));
        h.frame(build);
        REQUIRE_FALSE(h.context.needsUpdate(h.input));

        h.input.viewportSize = Vector2{640.0f, 480.0f};
        REQUIRE(h.context.needsUpdate(h.input));
    }
    SUBCASE("updates at a requested time")
    {
        h.input.timeSeconds = 10.0;
        h.frame([&](devgui::UI& ui) {
            build(ui);
            ui.requestUpdateAt(11.0);
        });
        h.input.timeSeconds = 10.5;
        REQUIRE_FALSE(h.context.needsUpdate(h.input));
        h.input.timeSeconds = 11.0;
        REQUIRE(h.context.needsUpdate(h.input));
    }
    SUBCASE("preserves invalidation requested while building a frame")
    {
        h.frame([&](devgui::UI& ui) {
            build(ui);
            h.context.invalidate();
        });
        REQUIRE(h.context.needsUpdate(h.input));
        h.frame(build);
        REQUIRE_FALSE(h.context.needsUpdate(h.input));
    }
    SUBCASE("preserves a style change requested while building a frame")
    {
        auto style = h.context.getStyle();
        style.textOnAccentColor = pomdog::Color{255, 80, 20, 255};
        h.frame([&](devgui::UI& ui) {
            build(ui);
            h.context.setStyle(style);
        });
        REQUIRE(h.context.needsUpdate(h.input));
        h.frame(build);
        REQUIRE_FALSE(h.context.needsUpdate(h.input));
        const auto& drawList = h.context.getDrawList();
        auto foundButtonText = false;
        for (const auto& command : drawList.getCommands()) {
            if ((command.kind == devgui::DrawCommandKind::Text) && (drawList.getText(command) == "OK")) {
                REQUIRE(command.color == style.textOnAccentColor);
                foundButtonText = true;
            }
        }
        REQUIRE(foundButtonText);
    }
}

TEST_CASE("devgui::collapsed window hit bounds")
{
    Harness h;
    h.context.setStyle(devgui::Style{});
    bool open = false;
    const auto build = [&](devgui::UI& ui) {
        auto options = windowAt(100.0f, 100.0f, 200.0f, 150.0f);
        options.collapsed = true;
        auto window = devgui::window(ui, "a"_key, "A", options);
        open = window.isOpen();
    };
    h.frame(build);
    REQUIRE_FALSE(open);
    // NOTE: This lies in the painted title but below a framed widget's height.
    h.moveTo(110.0f, 128.0f);
    REQUIRE(h.context.needsUpdate(h.input));
    h.frame(build);
    REQUIRE(h.context.getInputCapture().pointer);
    h.input.primaryButtonDown = true;
    REQUIRE(h.context.needsUpdate(h.input));
    h.frame(build);
    h.input.primaryButtonDown = false;
    REQUIRE(h.context.needsUpdate(h.input));
    h.frame(build);
    REQUIRE(open);
}

TEST_CASE("devgui::DrawList")
{
    Harness h;

    SUBCASE("owns copies of its text")
    {
        std::string label = "hello";
        h.frame([&](devgui::UI& ui) { devgui::text(ui, label); });
        label = "world";

        const auto& drawList = h.context.getDrawList();
        const auto commands = drawList.getCommands();
        REQUIRE(commands.size() == 1);
        REQUIRE(commands[0].kind == devgui::DrawCommandKind::Text);
        REQUIRE(drawList.getText(commands[0]) == "hello");
    }
    SUBCASE("culls shapes outside the clip rectangle")
    {
        h.frame([&](devgui::UI& ui) {
            ui.fillRect(pomdog::F32Rect2D{900.0f, 10.0f, 10.0f, 10.0f}, pomdog::Color{255, 0, 0, 255});
            ui.fillRect(pomdog::F32Rect2D{10.0f, 10.0f, 10.0f, 10.0f}, pomdog::Color{255, 0, 0, 255});
        });
        REQUIRE(h.context.getDrawList().getCommands().size() == 1);
    }
    SUBCASE("keeps a connected stroke when its third point or width enters the clip")
    {
        const auto color = pomdog::Color{240, 230, 220, 128};
        h.frame([&](devgui::UI& ui) {
            ui.pushClipRect(pomdog::F32Rect2D{10.0f, 10.0f, 10.0f, 10.0f});
            ui.drawPolyline(Vector2{1.0f, 12.0f}, Vector2{2.0f, 13.0f}, Vector2{11.0f, 14.0f}, color, 1.5f);
            ui.drawPolyline(Vector2{9.5f, 12.0f}, Vector2{9.5f, 14.0f}, Vector2{9.5f, 16.0f}, color, 1.5f);
            ui.drawPolyline(Vector2{6.0f, 12.0f}, Vector2{6.0f, 14.0f}, Vector2{6.0f, 16.0f}, color, 1.5f);
            ui.drawPolyline(Vector2{11.0f, 12.0f}, Vector2{12.0f, 14.0f}, Vector2{13.0f, 16.0f}, pomdog::Color{240, 230, 220, 0}, 1.5f);
            ui.popClipRect();
        });
        const auto& drawList = h.context.getDrawList();
        const auto commands = drawList.getCommands();
        REQUIRE(commands.size() == 2);
        REQUIRE(commands[0].kind == devgui::DrawCommandKind::Polyline);
        REQUIRE(commands[0].p0.x == doctest::Approx(1.0f));
        REQUIRE(commands[0].p1.x == doctest::Approx(2.0f));
        REQUIRE(commands[0].p2.x == doctest::Approx(11.0f));
        REQUIRE(commands[0].p2.y == doctest::Approx(14.0f));
        REQUIRE(commands[0].thickness == doctest::Approx(1.5f));
        REQUIRE(commands[0].color == color);
        REQUIRE(commands[0].clipIndex == commands[1].clipIndex);
        const auto& clip = drawList.getClipRects()[commands[0].clipIndex];
        REQUIRE(clip.x == doctest::Approx(10.0f));
        REQUIRE(clip.width == doctest::Approx(10.0f));
    }
}

TEST_CASE("devgui popups")
{
    // NOTE: "Open" is at (8, 6, 40, 20) and "Below" at (8, 30, 47, 20). The
    // popup opens 2 pixels below "Open" at (8, 28, 200, 100), so it covers
    // "Below". Its "Inside" button is at (16, 34, 54, 20).
    Harness h;
    const auto popupID = devgui::combineID(devgui::WidgetID{7}, "popup"_key);
    devgui::Response opener;
    devgui::Response below;
    devgui::Response inside;
    bool open = false;
    const auto build = [&](devgui::UI& ui) {
        opener = devgui::button(ui, "open"_key, "Open");
        below = devgui::button(ui, "below"_key, "Below");
        if (opener.clicked) {
            ui.openPopup(popupID);
        }
        open = ui.isPopupOpen(popupID);
        if (open) {
            ui.beginPopup(popupID, opener.rect, Vector2{200.0f, 100.0f});
            inside = devgui::button(ui, "inside"_key, "Inside");
            ui.endPopup();
        }
    };

    h.click(12.0f, 10.0f, build);
    REQUIRE(open);
    REQUIRE(inside.rect.x == 16.0f);
    REQUIRE(inside.rect.y == 34.0f);

    SUBCASE("takes the pointer from widgets beneath it")
    {
        h.moveTo(12.0f, 40.0f);
        h.frame(build);
        REQUIRE_FALSE(below.hovered);
        REQUIRE(h.context.getInputCapture().pointer);

        h.moveTo(30.0f, 40.0f);
        h.frame(build);
        REQUIRE(inside.hovered);
    }
    SUBCASE("closes on a press outside, which does nothing else")
    {
        h.moveTo(12.0f, 10.0f);
        h.input.primaryButtonDown = true;
        h.frame(build);
        REQUIRE_FALSE(open);
        REQUIRE_FALSE(opener.activated);

        h.input.primaryButtonDown = false;
        h.frame(build);
        REQUIRE_FALSE(opener.clicked);
        REQUIRE_FALSE(open);
    }
    SUBCASE("closes on Escape")
    {
        h.pressKey(devgui::EditKey::Escape, build);
        REQUIRE_FALSE(open);
    }
    SUBCASE("closes when a frame does not submit it")
    {
        h.frame([&](devgui::UI& ui) { open = ui.isPopupOpen(popupID); });
        REQUIRE(open);
        h.frame(build);
        REQUIRE_FALSE(open);
    }
}

TEST_CASE("devgui double clicks")
{
    Harness h;
    bool doubleClicked = false;
    const auto build = [&](devgui::UI& ui) {
        const auto r = devgui::button(ui, "ok"_key, "OK");
        doubleClicked = doubleClicked || r.doubleClicked;
    };
    const auto press = [&](f64 time) {
        doubleClicked = false;
        h.input.timeSeconds = time;
        h.input.primaryButtonDown = true;
        h.frame(build);
        h.input.primaryButtonDown = false;
        h.frame(build);
    };

    h.moveTo(12.0f, 10.0f);
    h.frame(build);

    SUBCASE("needs two quick presses")
    {
        press(1.0);
        REQUIRE_FALSE(doubleClicked);
        press(1.2);
        REQUIRE(doubleClicked);
        press(1.3);
        REQUIRE_FALSE(doubleClicked);
    }
    SUBCASE("ignores slow presses")
    {
        press(1.0);
        press(2.0);
        REQUIRE_FALSE(doubleClicked);
    }
}

TEST_CASE("devgui::UI text helpers")
{
    Harness h;

    SUBCASE("ends long text with an ellipsis")
    {
        // NOTE: The ellipsis is 3 bytes and 21 pixels wide, so 50 pixels fit
        // "abcd" before it.
        h.frame([&](devgui::UI& ui) {
            ui.drawText(Vector2{0.0f, 0.0f}, "abcdefghij", pomdog::Color{255, 255, 255, 255}, 50.0f);
        });
        const auto& drawList = h.context.getDrawList();
        REQUIRE(drawsText(drawList, "abcd"));
        REQUIRE(drawsText(drawList, "\xE2\x80\xA6"));
        REQUIRE_FALSE(drawsText(drawList, "abcdefghij"));
    }
    SUBCASE("keeps short text whole")
    {
        h.frame([&](devgui::UI& ui) {
            ui.drawText(Vector2{0.0f, 0.0f}, "abc", pomdog::Color{255, 255, 255, 255}, 50.0f);
        });
        REQUIRE(drawsText(h.context.getDrawList(), "abc"));
    }
    SUBCASE("wraps text once while it stays the same")
    {
        const auto id = devgui::WidgetID{42};
        constexpr std::string_view source = "one two three four";
        const char* first = nullptr;
        std::string wrapped;
        h.frame([&](devgui::UI& ui) {
            const auto result = ui.wrapText(id, source, 60.0f, pomdog::i18n::LineBreakLanguage::Default);
            first = result.data();
            wrapped = result;
        });
        REQUIRE(wrapped == "one two\nthree\nfour");

        const char* second = nullptr;
        h.frame([&](devgui::UI& ui) {
            second = ui.wrapText(id, source, 60.0f, pomdog::i18n::LineBreakLanguage::Default).data();
        });
        REQUIRE(first == second);
    }
    SUBCASE("keeps every wrapped view stable through a large frame")
    {
        constexpr std::string_view firstText = "FIRST";
        constexpr std::string_view otherText = "OTHER";
        constexpr std::string_view longText = "A long wrapped value that uses heap storage rather than a short string buffer.";
        std::array<const char*, 12> firstPointers;
        h.frame([&](devgui::UI& ui) {
            const auto first = ui.wrapText(devgui::WidgetID{1}, firstText, 1000.0f, pomdog::i18n::LineBreakLanguage::Default);
            const auto longView = ui.wrapText(devgui::WidgetID{2}, longText, 1000.0f, pomdog::i18n::LineBreakLanguage::Default);
            std::array<std::string_view, 12> views;
            for (std::size_t i = 0; i < views.size(); ++i) {
                views[i] = ui.wrapText(devgui::WidgetID{100 + i}, otherText, 1000.0f, pomdog::i18n::LineBreakLanguage::Default);
                firstPointers[i] = views[i].data();
            }
            REQUIRE(first == firstText);
            REQUIRE(longView == longText);
            for (const auto& view : views) {
                REQUIRE(view == otherText);
            }
        });
        h.frame([&](devgui::UI& ui) {
            for (std::size_t i = 0; i < firstPointers.size(); ++i) {
                const auto view = ui.wrapText(devgui::WidgetID{100 + i}, otherText, 1000.0f, pomdog::i18n::LineBreakLanguage::Default);
                REQUIRE(view.data() == firstPointers[i]);
            }
        });
    }
    SUBCASE("keeps earlier views when one widget changes its wrapping inputs")
    {
        constexpr std::string_view firstText = "FIRST";
        constexpr std::string_view otherText = "OTHER";
        constexpr std::string_view source = "one two three four";
        constexpr std::string_view wrappedText = "one two\nthree\nfour";
        h.frame([&](devgui::UI& ui) {
            const auto id = devgui::WidgetID{42};
            const auto first = ui.wrapText(id, firstText, 1000.0f, pomdog::i18n::LineBreakLanguage::Default);
            const auto other = ui.wrapText(id, otherText, 1000.0f, pomdog::i18n::LineBreakLanguage::Default);
            const auto whole = ui.wrapText(id, source, 1000.0f, pomdog::i18n::LineBreakLanguage::Default);
            const auto wrapped = ui.wrapText(id, source, 60.0f, pomdog::i18n::LineBreakLanguage::Default);
            REQUIRE(first == firstText);
            REQUIRE(other == otherText);
            REQUIRE(whole == source);
            REQUIRE(wrapped == wrappedText);
            const auto repeated = ui.wrapText(id, firstText, 1000.0f, pomdog::i18n::LineBreakLanguage::Default);
            REQUIRE(repeated.data() == first.data());
        });
    }
}
