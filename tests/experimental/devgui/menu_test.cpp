// Copyright mogemimi. Distributed under the MIT license.

#include "tests/experimental/devgui/devgui_test_harness.h"
#include "tests/testing/testing.h"
#include "pomdog/experimental/devgui/devgui.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <ostream>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace devgui = pomdog::devgui;
using pomdog::f32;
using pomdog::Vector2;
using namespace pomdog::devgui::literals;

using devgui_test::drawsText;
using devgui_test::Harness;

// NOTE: See devgui_test_harness.h for the positions these tests rely on. The
// "Target" button is at (8, 6, 54, 20). Menu rows are 20 pixels tall and keep
// a 20-pixel column for check marks, so a row with the label "Duplicate" and
// the shortcut "Ctrl+D" asks for 20 + 63 + 6 + 24 + 42 = 155 pixels. With the
// popup padding of (8, 6), the menu is 171 pixels wide.

TEST_CASE("devgui context menus")
{
    Harness h;
    devgui::Response target;
    devgui::Response duplicate;
    devgui::Response remove;
    devgui::Response disabled;
    bool menuOpen = false;
    const auto build = [&](devgui::UI& ui) {
        target = devgui::button(ui, "target"_key, "Target");
        auto menu = devgui::contextMenu(ui, target);
        menuOpen = menu.isOpen();
        if (menuOpen) {
            duplicate = devgui::menuItem(ui, "duplicate"_key, "Duplicate", {.shortcut = "Ctrl+D", .checked = false, .enabled = true});
            devgui::menuSeparator(ui);
            remove = devgui::menuItem(ui, "remove"_key, "Remove");
            disabled = devgui::menuItem(ui, "disabled"_key, "Disabled", {.shortcut = {}, .checked = false, .enabled = false});
        }
    };
    const auto secondaryClick = [&](f32 x, f32 y) {
        h.moveTo(x, y);
        h.frame(build);
        h.input.secondaryButtonDown = true;
        h.frame(build);
        h.input.secondaryButtonDown = false;
        h.frame(build);
    };

    SUBCASE("opens where the secondary button is released over the widget")
    {
        secondaryClick(20.0f, 10.0f);
        REQUIRE(target.secondaryClicked);
        REQUIRE(menuOpen);

        // NOTE: The first frame measures the menu without showing it.
        REQUIRE_FALSE(drawsText(h.context.getDrawList(), "Duplicate"));
        REQUIRE(h.context.needsUpdate(h.input));

        h.frame(build);
        REQUIRE(drawsText(h.context.getDrawList(), "Duplicate"));
        REQUIRE(drawsText(h.context.getDrawList(), "Ctrl+D"));
        REQUIRE(duplicate.rect.x == 28.0f);
        REQUIRE(duplicate.rect.y == 18.0f);
        REQUIRE(duplicate.rect.width == 155.0f);

        // NOTE: The separator takes 4 pixels between two item spacings.
        REQUIRE(remove.rect.y == 18.0f + 20.0f + 4.0f + 4.0f + 4.0f);
    }
    SUBCASE("needs the press and the release on the same widget")
    {
        h.moveTo(300.0f, 300.0f);
        h.frame(build);
        h.input.secondaryButtonDown = true;
        h.frame(build);
        h.moveTo(20.0f, 10.0f);
        h.frame(build);
        h.input.secondaryButtonDown = false;
        h.frame(build);
        REQUIRE_FALSE(target.secondaryClicked);
        REQUIRE_FALSE(menuOpen);
    }
    SUBCASE("closes after the user chooses an item")
    {
        secondaryClick(20.0f, 10.0f);
        h.frame(build);
        h.click(40.0f, 28.0f, build);
        REQUIRE(duplicate.clicked);
        h.frame(build);
        REQUIRE_FALSE(menuOpen);
    }
    SUBCASE("ignores clicks on a disabled item")
    {
        secondaryClick(20.0f, 10.0f);
        h.frame(build);
        const auto y = disabled.rect.y + 5.0f;
        h.click(40.0f, y, build);
        REQUIRE_FALSE(disabled.clicked);
        REQUIRE(menuOpen);
    }
    SUBCASE("moves to where the secondary button is released again")
    {
        secondaryClick(20.0f, 10.0f);
        h.frame(build);

        // NOTE: (14, 20) is on "Target" but outside the menu at (20, 12).
        secondaryClick(14.0f, 20.0f);
        REQUIRE(menuOpen);
        REQUIRE(duplicate.rect.x == 22.0f);
        REQUIRE(duplicate.rect.y == 28.0f);
    }
    SUBCASE("closes on a press outside, which does nothing else")
    {
        secondaryClick(20.0f, 10.0f);
        h.frame(build);
        h.moveTo(400.0f, 400.0f);
        h.input.primaryButtonDown = true;
        h.frame(build);
        REQUIRE_FALSE(menuOpen);
        h.input.primaryButtonDown = false;
        h.frame(build);
        REQUIRE_FALSE(menuOpen);
    }
}

TEST_CASE("devgui submenus")
{
    Harness h;
    devgui::Response target;
    devgui::Response first;
    devgui::Response nested;
    devgui::Response last;
    bool subMenuOpen = false;
    const auto build = [&](devgui::UI& ui) {
        target = devgui::button(ui, "target"_key, "Target");
        if (auto menu = devgui::contextMenu(ui, target); menu.isOpen()) {
            first = devgui::menuItem(ui, "first"_key, "First");
            subMenuOpen = false;
            if (auto sub = devgui::subMenu(ui, "more"_key, "More"); sub.isOpen()) {
                subMenuOpen = true;
                nested = devgui::menuItem(ui, "nested"_key, "Nested");
            }
            last = devgui::menuItem(ui, "last"_key, "Last");
        }
        else {
            subMenuOpen = false;
        }
    };

    h.moveTo(20.0f, 10.0f);
    h.frame(build);
    h.input.secondaryButtonDown = true;
    h.frame(build);
    h.input.secondaryButtonDown = false;
    h.frame(build);
    h.frame(build);

    // NOTE: "More" is the second row of the menu at (20, 12).
    const auto moreY = first.rect.y + first.rect.height + 4.0f;
    const auto restOn = [&](f32 x, f32 y) {
        h.moveTo(x, y);
        h.frame(build);
        h.input.timeSeconds += 0.2;
        h.frame(build);
        h.frame(build);
    };

    SUBCASE("opens beside its item after the pointer rests on it")
    {
        h.moveTo(40.0f, moreY + 5.0f);
        h.frame(build);
        REQUIRE_FALSE(subMenuOpen);
        REQUIRE_FALSE(h.context.needsUpdate(h.input));

        // NOTE: The item asked for a frame when the delay passes.
        h.input.timeSeconds += 0.2;
        REQUIRE(h.context.needsUpdate(h.input));
        h.frame(build);
        REQUIRE(subMenuOpen);
        h.frame(build);

        // NOTE: The submenu's first row lines up with "More", and the submenu
        // starts at the menu's right edge.
        REQUIRE(nested.rect.y == moreY);
        REQUIRE(nested.rect.x == first.rect.x + first.rect.width + 8.0f + 8.0f);
    }
    SUBCASE("closes when the pointer rests on another item of its menu")
    {
        restOn(40.0f, moreY + 5.0f);
        REQUIRE(subMenuOpen);

        h.moveTo(40.0f, last.rect.y + 5.0f);
        h.frame(build);
        REQUIRE(subMenuOpen);
        h.input.timeSeconds += 0.2;
        h.frame(build);
        h.frame(build);
        REQUIRE_FALSE(subMenuOpen);
    }
    SUBCASE("closes alone on Escape")
    {
        restOn(40.0f, moreY + 5.0f);
        REQUIRE(subMenuOpen);

        // NOTE: Leave "More" first, or resting on it would open the submenu
        // again.
        h.moveTo(nested.rect.x + 10.0f, nested.rect.y + 5.0f);
        h.frame(build);
        h.pressKey(devgui::EditKey::Escape, build);
        REQUIRE_FALSE(subMenuOpen);
        REQUIRE(drawsText(h.context.getDrawList(), "First"));
    }
    SUBCASE("closes every menu when the user chooses a nested item")
    {
        restOn(40.0f, moreY + 5.0f);
        h.click(nested.rect.x + 10.0f, nested.rect.y + 5.0f, build);
        REQUIRE(nested.clicked);
        h.frame(build);
        REQUIRE_FALSE(drawsText(h.context.getDrawList(), "First"));
    }
}

TEST_CASE("devgui popups sized to fit their content")
{
    Harness h;
    const auto popupID = devgui::combineID(devgui::WidgetID{7}, "popup"_key);
    devgui::Response item;
    const auto build = [&](devgui::UI& ui) {
        const auto opener = devgui::button(ui, "open"_key, "Open");
        if (opener.clicked) {
            ui.openPopup(popupID);
        }
        if (ui.isPopupOpen(popupID)) {
            ui.beginPopup(popupID, opener.rect, Vector2{0.0f, 0.0f});
            item = devgui::button(ui, "wide"_key, "A wide button label");
            ui.endPopup();
        }
    };

    h.click(12.0f, 10.0f, build);
    h.frame(build);

    // NOTE: The button is 19 * 7 + 12 = 145 pixels wide, so the popup is
    // 161 pixels wide and 32 pixels tall below "Open" at (8, 6, 40, 20).
    REQUIRE(item.rect.x == 16.0f);
    REQUIRE(item.rect.y == 34.0f);
    h.moveTo(8.0f + 160.0f, 28.0f + 31.0f);
    h.frame(build);
    REQUIRE(h.context.getInputCapture().pointer);
    h.moveTo(8.0f + 162.0f, 28.0f + 31.0f);
    h.frame(build);
    REQUIRE_FALSE(h.context.getInputCapture().pointer);
}
