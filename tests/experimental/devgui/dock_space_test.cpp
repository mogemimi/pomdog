// Copyright mogemimi. Distributed under the MIT license.

#include "tests/experimental/devgui/devgui_test_harness.h"
#include "tests/testing/testing.h"
#include "pomdog/experimental/devgui/devgui.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <ostream>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace devgui = pomdog::devgui;
using pomdog::f32;
using pomdog::F32Rect2D;
using pomdog::Vector2;
using namespace pomdog::devgui::literals;

using devgui_test::drawsText;
using devgui_test::Harness;
using devgui_test::windowAt;

// NOTE: See devgui_test_harness.h for the style these tests use. The dock
// space fills the 800 by 600 viewport. Its root splits into a left node
// 200 pixels wide, a 2-pixel gap, and a right node from x = 202. Tab bars are
// 20 pixels tall like title bars, so the content of a docked window starts
// 8 pixels right of its node and 26 pixels below its top. Tabs with one-letter
// titles take their minimum width of 40 pixels.

namespace {

/// Builds a dock space with the windows A, B, C, and D, each showing a button.
class DockFixture final {
public:
    Harness h;
    devgui::Response a;
    devgui::Response b;
    devgui::Response c;
    devgui::Response d;
    bool aOpen = false;
    bool bOpen = false;
    bool cOpen = false;
    bool dOpen = false;
    bool submitD = false;

    /// True to give the windows close buttons, which clear the flags below.
    bool closable = false;
    bool aShown = true;
    bool bShown = true;
    bool cShown = true;
    bool dShown = true;
    bool showTabBars = true;

    DockFixture() = default;
    DockFixture(const DockFixture&) = delete;
    DockFixture& operator=(const DockFixture&) = delete;
    DockFixture(DockFixture&&) = delete;
    DockFixture& operator=(DockFixture&&) = delete;
    ~DockFixture() = default;

    /// Docks A and B in the left node, and C in the right node.
    void buildDefaultLayout()
    {
        devgui::DockLayoutBuilder layout{h.context, "dock"_key};
        const auto [left, right] = layout.split(layout.root(), devgui::DockSplit::LeftRight, 0.25f);
        layout.dock(left, "a"_key);
        layout.dock(left, "b"_key);
        layout.dock(right, "c"_key);
    }

    void build(devgui::UI& ui)
    {
        devgui::dockSpace(ui, "dock"_key, F32Rect2D{0.0f, 0.0f, 800.0f, 600.0f}, devgui::DockSpaceOptions{.showTabBars = showTabBars});
        aOpen = submit(ui, "a"_key, "A", windowAt(100.0f, 100.0f, 200.0f, 150.0f), a, aShown);
        bOpen = submit(ui, "b"_key, "B", windowAt(100.0f, 100.0f, 200.0f, 150.0f), b, bShown);
        cOpen = submit(ui, "c"_key, "C", windowAt(100.0f, 100.0f, 200.0f, 150.0f), c, cShown);
        if (submitD) {
            dOpen = submit(ui, "d"_key, "D", windowAt(500.0f, 400.0f, 200.0f, 150.0f), d, dShown);
        }
    }

    void frame()
    {
        h.frame([this](devgui::UI& ui) { build(ui); });
    }

    void moveTo(f32 x, f32 y)
    {
        h.moveTo(x, y);
        frame();
    }

    void press()
    {
        h.input.primaryButtonDown = true;
        frame();
    }

    void release()
    {
        h.input.primaryButtonDown = false;
        frame();
    }

private:
    [[nodiscard]] bool
    submit(devgui::UI& ui, devgui::Key key, std::string_view title, const devgui::WindowOptions& options, devgui::Response& response, bool& shown) const
    {
        if (closable && !shown) {
            return false;
        }
#if defined(_MSC_VER) && _MSC_VER >= 1930 && _MSC_VER < 1950
        // NOTE: Visual Studio 2022 rejects a conditional operator whose operands
        // are a scope that cannot move, so each branch passes its own scope.
        const auto fill = [&](const devgui::WindowScope& window) {
            if (window.isOpen()) {
                response = devgui::button(ui, "button"_key, "Button");
            }
            return window.isOpen();
        };
        if (closable) {
            return fill(devgui::window(ui, key, title, shown, options));
        }
        return fill(devgui::window(ui, key, title, options));
#else
        auto window = closable ? devgui::window(ui, key, title, shown, options) : devgui::window(ui, key, title, options);
        if (window.isOpen()) {
            response = devgui::button(ui, "button"_key, "Button");
        }
        return window.isOpen();
#endif
    }
};

} // namespace

TEST_CASE("devgui dock spaces")
{
    DockFixture f;
    f.buildDefaultLayout();
    REQUIRE(f.h.context.hasDockSpace("dock"_key));
    REQUIRE_FALSE(f.h.context.hasDockSpace("other"_key));

    // NOTE: Tab bars show windows from the frame after they are submitted.
    f.frame();
    f.frame();

    SUBCASE("shows the selected window of each node")
    {
        REQUIRE(f.aOpen);
        REQUIRE_FALSE(f.bOpen);
        REQUIRE(f.cOpen);
        REQUIRE(f.a.rect.x == 8.0f);
        REQUIRE(f.a.rect.y == 26.0f);
        REQUIRE(f.c.rect.x == 210.0f);
        REQUIRE(f.c.rect.y == 26.0f);
        const auto& drawList = f.h.context.getDrawList();
        REQUIRE(drawsText(drawList, "A"));
        REQUIRE(drawsText(drawList, "B"));
        REQUIRE(drawsText(drawList, "C"));
    }
    SUBCASE("selects a window by its tab")
    {
        f.moveTo(50.0f, 10.0f);
        f.press();
        f.release();
        REQUIRE_FALSE(f.aOpen);
        REQUIRE(f.bOpen);
        REQUIRE(f.b.rect.x == 8.0f);
        REQUIRE(f.b.rect.y == 26.0f);
    }
    SUBCASE("resizes nodes by dragging the gap between them")
    {
        f.moveTo(201.0f, 300.0f);
        f.press();
        f.moveTo(251.0f, 300.0f);
        REQUIRE(f.c.rect.x == 260.0f);
        f.release();
        f.frame();
        REQUIRE(f.c.rect.x == 260.0f);
    }
    SUBCASE("hides the tab of a window that is not submitted")
    {
        f.h.frame([&](devgui::UI& ui) {
            devgui::dockSpace(ui, "dock"_key, F32Rect2D{0.0f, 0.0f, 800.0f, 600.0f});
            f.aOpen = devgui::window(ui, "a"_key, "A").isOpen();
        });
        REQUIRE(f.h.context.needsUpdate(f.h.input));
        f.h.frame([&](devgui::UI& ui) {
            devgui::dockSpace(ui, "dock"_key, F32Rect2D{0.0f, 0.0f, 800.0f, 600.0f});
            f.aOpen = devgui::window(ui, "a"_key, "A").isOpen();
        });
        const auto& drawList = f.h.context.getDrawList();
        REQUIRE(drawsText(drawList, "A"));
        REQUIRE_FALSE(drawsText(drawList, "B"));
        REQUIRE_FALSE(drawsText(drawList, "C"));
    }
    SUBCASE("floats a window dragged out by its tab, and docks it beside a node")
    {
        // NOTE: The pointer holds tab B 10 pixels from its left edge and keeps
        // that grip on the title bar of the floating window.
        f.moveTo(50.0f, 10.0f);
        f.press();
        f.moveTo(300.0f, 300.0f);
        REQUIRE(f.aOpen);
        REQUIRE(f.bOpen);
        REQUIRE(f.b.rect.x == 298.0f);
        REQUIRE(f.b.rect.y == 316.0f);

        // NOTE: Dropping B near the right edge of C's node splits the node.
        // B takes its width, 200 of 598 pixels, so C keeps
        // round(596 * (1 - 200 / 598)) = 397 pixels.
        f.moveTo(790.0f, 300.0f);
        f.release();
        f.frame();
        REQUIRE(f.bOpen);
        REQUIRE(f.b.rect.x == 609.0f);
        REQUIRE(f.b.rect.y == 26.0f);
        REQUIRE(f.c.rect.x == 210.0f);
        REQUIRE(f.aOpen);
    }
    SUBCASE("docks a floating window dropped in the middle of a node as a tab")
    {
        f.submitD = true;
        f.frame();
        REQUIRE(f.dOpen);
        REQUIRE(f.d.rect.x == 508.0f);

        f.moveTo(550.0f, 410.0f);
        f.press();
        f.moveTo(100.0f, 300.0f);
        f.release();
        f.frame();
        REQUIRE(f.dOpen);
        REQUIRE_FALSE(f.aOpen);
        REQUIRE(f.d.rect.x == 8.0f);
        REQUIRE(f.d.rect.y == 26.0f);
        REQUIRE(drawsText(f.h.context.getDrawList(), "D"));
    }
    SUBCASE("does not dock a window whose title bar is only clicked")
    {
        f.submitD = true;
        f.frame();

        // NOTE: D floats over C's node.
        f.moveTo(550.0f, 410.0f);
        f.press();
        f.release();
        f.frame();
        REQUIRE(f.dOpen);
        REQUIRE(f.d.rect.x == 508.0f);
        REQUIRE(f.cOpen);
    }
}

TEST_CASE("devgui layouts")
{
    DockFixture f;
    f.buildDefaultLayout();
    f.frame();
    f.frame();

    // NOTE: Drag the gap so that the saved layout differs from the default.
    f.moveTo(201.0f, 300.0f);
    f.press();
    f.moveTo(251.0f, 300.0f);
    f.release();
    REQUIRE(f.c.rect.x == 260.0f);

    const auto text = f.h.context.saveLayout();
    REQUIRE(text.starts_with("devgui-layout 1\n"));

    SUBCASE("restores the docked windows and splits")
    {
        DockFixture g;
        const bool loaded = (g.h.context.loadLayout(text) == nullptr);
        REQUIRE(loaded);
        REQUIRE(g.h.context.hasDockSpace("dock"_key));
        g.frame();
        g.frame();
        REQUIRE(g.aOpen);
        REQUIRE_FALSE(g.bOpen);
        REQUIRE(g.cOpen);
        REQUIRE(g.a.rect.x == 8.0f);
        REQUIRE(g.c.rect.x == 260.0f);
        REQUIRE(g.h.context.saveLayout() == text);
    }
    SUBCASE("restores floating windows")
    {
        f.submitD = true;
        f.frame();
        const auto withD = f.h.context.saveLayout();

        DockFixture g;
        const bool loaded = (g.h.context.loadLayout(withD) == nullptr);
        REQUIRE(loaded);
        g.submitD = true;
        g.frame();
        REQUIRE(g.dOpen);
        REQUIRE(g.d.rect.x == 508.0f);
        REQUIRE(g.d.rect.y == 426.0f);
    }
    SUBCASE("rejects invalid text and keeps the current layout")
    {
        // NOTE: A split without its second child, a missing header, a
        // negative size, and a window docked twice.
        constexpr std::array<const char*, 4> invalidLayouts = {{
            "devgui-layout 1\ndock 1 0\nsplit 0 5000\nleaf -1 0\n",
            "not a layout",
            "devgui-layout 1\nwindow 1 0 0 0 -5 10 0\n",
            "devgui-layout 1\ndock 1 0\nleaf 0 1 2 0\ndock 3 0\nleaf 0 1 2 0\n",
        }};
        for (const auto* invalid : invalidLayouts) {
            const bool failed = (f.h.context.loadLayout(invalid) != nullptr);
            REQUIRE(failed);
        }
        REQUIRE(f.h.context.saveLayout() == text);
    }
}

TEST_CASE("devgui window close buttons")
{
    DockFixture f;
    f.closable = true;
    f.buildDefaultLayout();
    f.frame();
    f.frame();

    // NOTE: With close buttons the one-letter tabs still take 40 pixels. A
    // tab's 12-pixel close button ends 4.5 pixels before the tab's right edge,
    // so tab A's spans x = 23.5 to 35.5 and tab B's x = 63.5 to 75.5, from
    // y = 4 to 16.
    SUBCASE("closes a docked window from its tab and brings it back there")
    {
        // NOTE: The tab beside the closed one is selected at once.
        f.moveTo(29.0f, 10.0f);
        f.press();
        f.release();
        REQUIRE_FALSE(f.aShown);
        REQUIRE_FALSE(f.aOpen);
        REQUIRE(f.bOpen);
        REQUIRE(f.b.rect.x == 8.0f);

        REQUIRE(f.h.context.needsUpdate(f.h.input));
        f.frame();
        REQUIRE(f.h.context.needsUpdate(f.h.input));
        f.frame();
        REQUIRE_FALSE(drawsText(f.h.context.getDrawList(), "A"));
        REQUIRE(f.bOpen);
        REQUIRE_FALSE(f.h.context.needsUpdate(f.h.input));

        f.aShown = true;
        f.frame();
        f.frame();
        REQUIRE(drawsText(f.h.context.getDrawList(), "A"));
        REQUIRE(f.bOpen);
        REQUIRE_FALSE(f.aOpen);
    }
    SUBCASE("closes a tab that is not selected without showing it")
    {
        f.moveTo(69.0f, 10.0f);
        f.press();
        f.release();
        REQUIRE_FALSE(f.bShown);
        REQUIRE(f.aOpen);
        REQUIRE(f.aShown);

        REQUIRE(f.h.context.needsUpdate(f.h.input));
        f.frame();
        REQUIRE(f.h.context.needsUpdate(f.h.input));
        f.frame();
        REQUIRE_FALSE(drawsText(f.h.context.getDrawList(), "B"));
        REQUIRE_FALSE(f.h.context.needsUpdate(f.h.input));
    }
    SUBCASE("closes a floating window from its title bar")
    {
        // NOTE: D's close button is the last 20 pixels of its title bar.
        f.submitD = true;
        f.frame();
        f.moveTo(690.0f, 410.0f);
        f.press();
        f.release();
        REQUIRE_FALSE(f.dShown);
        REQUIRE(f.d.rect.x == 508.0f);

        REQUIRE(f.h.context.needsUpdate(f.h.input));
        f.frame();
        REQUIRE_FALSE(drawsText(f.h.context.getDrawList(), "D"));
        REQUIRE_FALSE(f.h.context.needsUpdate(f.h.input));
    }
}

TEST_CASE("devgui dock space queries")
{
    DockFixture f;
    f.buildDefaultLayout();
    f.frame();
    f.frame();

    std::vector<devgui::DockedWindow> docked;
    f.h.context.getDockedWindows("dock"_key, docked);
    REQUIRE(docked.size() == 3);
    REQUIRE(docked[0].window == "a"_key);
    REQUIRE(docked[0].selected);
    REQUIRE(docked[1].window == "b"_key);
    REQUIRE_FALSE(docked[1].selected);
    REQUIRE(docked[0].node == docked[1].node);
    REQUIRE(docked[2].window == "c"_key);
    REQUIRE(docked[2].selected);
    REQUIRE_FALSE(docked[2].node == docked[0].node);

    SUBCASE("selects a docked window by key")
    {
        f.h.context.selectWindow("b"_key);
        REQUIRE(f.h.context.needsUpdate(f.h.input));
        f.frame();
        REQUIRE(f.bOpen);
        REQUIRE_FALSE(f.aOpen);
    }
    SUBCASE("moves a tab to where another tab is")
    {
        f.h.context.moveWindowTab("b"_key, "a"_key);
        f.h.context.getDockedWindows("dock"_key, docked);
        REQUIRE(docked.size() == 3);
        REQUIRE(docked[0].window == "b"_key);
        REQUIRE(docked[1].window == "a"_key);

        // NOTE: Windows in different nodes keep their tabs.
        f.h.context.moveWindowTab("c"_key, "a"_key);
        f.h.context.getDockedWindows("dock"_key, docked);
        REQUIRE(docked[2].window == "c"_key);
    }
    SUBCASE("lists only windows whose tabs show")
    {
        f.h.frame([&](devgui::UI& ui) {
            devgui::dockSpace(ui, "dock"_key, F32Rect2D{0.0f, 0.0f, 800.0f, 600.0f});
            f.aOpen = devgui::window(ui, "a"_key, "A").isOpen();
        });
        f.h.context.getDockedWindows("dock"_key, docked);
        REQUIRE(docked.size() == 1);
        REQUIRE(docked[0].window == "a"_key);
    }
    SUBCASE("hides the tab bars")
    {
        f.showTabBars = false;
        f.frame();
        REQUIRE(f.a.rect.y == 6.0f);
        REQUIRE(f.c.rect.y == 6.0f);
        REQUIRE_FALSE(drawsText(f.h.context.getDrawList(), "B"));
        f.frame();
        REQUIRE(f.a.rect.y == 6.0f);
    }
}

TEST_CASE("devgui dock tab reordering")
{
    DockFixture f;
    f.buildDefaultLayout();
    f.frame();
    f.frame();

    // NOTE: Tab A spans x = 0 to 40 and tab B x = 40 to 80. Dropping A right of
    // the middle of B moves A after B.
    f.moveTo(20.0f, 10.0f);
    f.press();
    f.moveTo(70.0f, 10.0f);

    std::vector<devgui::DockedWindow> docked;
    f.h.context.getDockedWindows("dock"_key, docked);
    REQUIRE(docked[0].window == "a"_key);
    REQUIRE(drawsText(f.h.context.getDrawList(), "A"));

    f.release();
    f.h.context.getDockedWindows("dock"_key, docked);
    REQUIRE(docked.size() == 3);
    REQUIRE(docked[0].window == "b"_key);
    REQUIRE(docked[1].window == "a"_key);
    REQUIRE(docked[1].selected);
}
