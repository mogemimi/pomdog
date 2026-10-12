// Copyright mogemimi. Distributed under the MIT license.

#include "tests/experimental/devgui/devgui_test_harness.h"
#include "tests/testing/testing.h"
#include "pomdog/experimental/devgui/devgui.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <limits>
#include <optional>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace devgui = pomdog::devgui;
using devgui_test::drawsText;
using devgui_test::Harness;
using pomdog::Color;
using pomdog::f32;
using pomdog::i32;
using pomdog::u64;
using pomdog::Vector2;
using namespace pomdog::devgui::literals;

// NOTE: See devgui_test_harness.h for the positions these tests rely on. Every
// widget here sits outside any window, in the property row at y = 6 to 26 whose
// control spans x = 321 to 792. Popups open 2 pixels below the control, at
// y = 28, with 8 and 6 pixels of padding.

TEST_CASE("devgui::combo")
{
    Harness h;
    constexpr std::array<std::string_view, 3> items = {{"Alpha", "Beta", "Gamma"}};
    i32 selected = 0;
    devgui::Response r;
    const auto build = [&](devgui::UI& ui) { r = devgui::combo(ui, "shape"_key, "Shape", selected, items); };

    h.frame(build);
    REQUIRE(drawsText(h.context.getDrawList(), "Alpha"));
    REQUIRE_FALSE(drawsText(h.context.getDrawList(), "Gamma"));

    h.click(400.0f, 16.0f, build);
    REQUIRE(drawsText(h.context.getDrawList(), "Gamma"));

    SUBCASE("selects the clicked item")
    {
        // NOTE: Items are 20 pixels tall with 4 pixels between them, starting at
        // y = 34, so "Gamma" spans y = 82 to 102.
        h.moveTo(400.0f, 90.0f);
        h.frame(build);
        h.input.primaryButtonDown = true;
        h.frame(build);
        h.input.primaryButtonDown = false;
        h.frame(build);
        REQUIRE(r.changed);
        REQUIRE(selected == 2);

        h.frame(build);
        REQUIRE_FALSE(r.changed);
        REQUIRE_FALSE(drawsText(h.context.getDrawList(), "Beta"));
        REQUIRE(drawsText(h.context.getDrawList(), "Gamma"));
    }
    SUBCASE("closes when the box is clicked again")
    {
        h.click(400.0f, 16.0f, build);
        REQUIRE_FALSE(drawsText(h.context.getDrawList(), "Beta"));
        REQUIRE(selected == 0);
    }
}

TEST_CASE("devgui::colorEdit")
{
    // NOTE: The 40-pixel swatch at x = 321 opens a 260-pixel picker at
    // (321, 28). Its saturation and value square is at (329, 34, 220, 160)
    // and its hue bar at (555, 34, 18, 160).
    Harness h;
    auto value = Color{128, 128, 128, 255};
    devgui::Response r;
    const auto build = [&](devgui::UI& ui) { r = devgui::colorEdit(ui, "tint"_key, "Tint", value); };

    h.frame(build);
    REQUIRE(drawsText(h.context.getDrawList(), "#808080FF"));

    h.click(330.0f, 16.0f, build);

    // NOTE: Dragging past the top-right corner of the square selects full
    // saturation and value at the hue of gray, which is red.
    h.moveTo(400.0f, 100.0f);
    h.frame(build);
    h.input.primaryButtonDown = true;
    h.frame(build);
    h.moveTo(700.0f, 0.0f);
    h.frame(build);
    REQUIRE(r.changed);
    REQUIRE(value == Color{255, 0, 0, 255});
    h.input.primaryButtonDown = false;
    h.frame(build);

    SUBCASE("changes the hue")
    {
        // NOTE: A third of the way down the hue bar is green.
        h.moveTo(564.0f, 34.0f + 160.0f / 3.0f);
        h.frame(build);
        h.input.primaryButtonDown = true;
        h.frame(build);
        REQUIRE(value == Color{0, 255, 0, 255});
    }
    SUBCASE("keeps the hue at zero value")
    {
        h.click(564.0f, 34.0f + 160.0f / 3.0f, build);
        REQUIRE(value == Color{0, 255, 0, 255});

        // NOTE: Dragging to the bottom of the square makes black, from which
        // RGB alone cannot recover the hue. Moving back up restores green.
        h.moveTo(500.0f, 100.0f);
        h.frame(build);
        h.input.primaryButtonDown = true;
        h.frame(build);
        h.moveTo(700.0f, 300.0f);
        h.frame(build);
        REQUIRE(value == Color{0, 0, 0, 255});
        h.moveTo(700.0f, 0.0f);
        h.frame(build);
        REQUIRE(value == Color{0, 255, 0, 255});
    }
}

TEST_CASE("devgui::inputText")
{
    // NOTE: The field spans x = 321 to 792, and its text starts at x = 327.
    Harness h;
    std::string value;
    devgui::Response r;
    bool activated = false;
    bool deactivated = false;
    const auto build = [&](devgui::UI& ui) {
        r = devgui::inputText(ui, "name"_key, "Name", value, devgui::InputTextOptions{.placeholder = "Untitled"});
        activated = activated || r.activated;
        deactivated = deactivated || r.deactivated;
    };
    const auto type = [&](std::string_view text) {
        h.input.textInput = text;
        h.frame(build);
    };

    h.frame(build);
    REQUIRE(drawsText(h.context.getDrawList(), "Untitled"));

    h.click(400.0f, 16.0f, build);
    REQUIRE(activated);
    REQUIRE(r.active);
    REQUIRE(h.context.getInputCapture().keyboard);

    SUBCASE("edits text with typing and keys")
    {
        type("abc");
        REQUIRE(value == "abc");
        REQUIRE(r.changed);

        h.pressKey(devgui::EditKey::Backspace, build);
        REQUIRE(value == "ab");

        h.pressKey(devgui::EditKey::LeftArrow, build);
        type("X");
        REQUIRE(value == "aXb");

        h.pressKey(devgui::EditKey::Home, build);
        h.pressKey(devgui::EditKey::Delete, build);
        REQUIRE(value == "Xb");
    }
    SUBCASE("ignores control characters in typed text")
    {
        type("a\bb\r\n");
        REQUIRE(value == "ab");
    }
    SUBCASE("keeps editing on character boundaries after an external value replacement")
    {
        type("ab");
        h.pressKey(devgui::EditKey::LeftArrow, build);

        // NOTE: U+732B, "cat" in Japanese. Offset 1 from the old ASCII value
        // lies inside this three-byte character and must move back to zero.
        const auto replaceValue = [&] {
            value = "\xE7\x8C\xAB";
            h.context.invalidate();
        };

        SUBCASE("inserts before the complete replacement character")
        {
            replaceValue();
            type("x");
            REQUIRE(value == "x\xE7\x8C\xAB");
        }
        SUBCASE("does not backspace through part of the replacement character")
        {
            replaceValue();
            h.pressKey(devgui::EditKey::Backspace, build);
            REQUIRE(value == "\xE7\x8C\xAB");
        }
        SUBCASE("deletes the complete replacement character")
        {
            replaceValue();
            h.pressKey(devgui::EditKey::Delete, build);
            REQUIRE(value.empty());
        }
        SUBCASE("clamps both ends of an old selection")
        {
            h.input.shiftDown = true;
            h.pressKey(devgui::EditKey::RightArrow, build);
            h.input.shiftDown = false;
            replaceValue();
            type("x");
            REQUIRE(value == "x\xE7\x8C\xAB");
        }
    }
    SUBCASE("replaces the selection")
    {
        type("hello");
        h.input.shiftDown = true;
        h.pressKey(devgui::EditKey::Home, build);
        h.input.shiftDown = false;
        type("bye");
        REQUIRE(value == "bye");

        h.input.shortcutDown = true;
        h.pressKey(devgui::EditKey::A, build);
        h.input.shortcutDown = false;
        h.pressKey(devgui::EditKey::Backspace, build);
        REQUIRE(value.empty());
    }
    SUBCASE("erases a Thai character with its marks")
    {
        // NOTE: U+0E01 THAI CHARACTER KO KAI followed by U+0E35 THAI CHARACTER
        // SARA II, which sits above it.
        type("\xE0\xB8\x81\xE0\xB8\xB5");
        h.pressKey(devgui::EditKey::Backspace, build);
        REQUIRE(value.empty());
    }
    SUBCASE("erases a decomposed kana and a keycap whole")
    {
        // NOTE: U+30AB KATAKANA LETTER KA with U+3099 COMBINING
        // KATAKANA-HIRAGANA VOICED SOUND MARK, then the keycap digit one: "1",
        // U+FE0F VARIATION SELECTOR-16, and U+20E3 COMBINING ENCLOSING KEYCAP.
        type("\xE3\x82\xAB\xE3\x82\x99"
             "1\xEF\xB8\x8F\xE2\x83\xA3");
        h.pressKey(devgui::EditKey::Backspace, build);
        REQUIRE(value == "\xE3\x82\xAB\xE3\x82\x99");
        h.pressKey(devgui::EditKey::Backspace, build);
        REQUIRE(value.empty());
    }
    SUBCASE("moves over a flag and deletes it whole")
    {
        // NOTE: The flags of Japan (U+1F1EF U+1F1F5) and the United States
        // (U+1F1FA U+1F1F8).
        type("\xF0\x9F\x87\xAF\xF0\x9F\x87\xB5\xF0\x9F\x87\xBA\xF0\x9F\x87\xB8");
        h.pressKey(devgui::EditKey::LeftArrow, build);
        h.pressKey(devgui::EditKey::Delete, build);
        REQUIRE(value == "\xF0\x9F\x87\xAF\xF0\x9F\x87\xB5");
        h.pressKey(devgui::EditKey::Home, build);
        h.pressKey(devgui::EditKey::RightArrow, build);
        h.pressKey(devgui::EditKey::Backspace, build);
        REQUIRE(value.empty());
    }
    SUBCASE("places the caret where clicked")
    {
        // NOTE: Each letter is 7 pixels wide, so x = 327 + 15 is nearest to the
        // boundary after "he".
        type("hello");
        h.click(342.0f, 16.0f, build);
        type("_");
        REQUIRE(value == "he_llo");
    }
    SUBCASE("repeats a held key")
    {
        type("abcdef");
        h.input.timeSeconds = 1.0;
        h.input.setKeyDown(devgui::EditKey::Backspace, true);
        h.frame(build);
        REQUIRE(value == "abcde");

        h.input.timeSeconds = 1.3;
        h.frame(build);
        REQUIRE(value == "abcde");

        // NOTE: The context wakes up for the first repeat.
        h.input.timeSeconds = 1.46;
        REQUIRE(h.context.needsUpdate(h.input));
        h.frame(build);
        REQUIRE(value == "abcd");

        h.input.timeSeconds = 1.5;
        h.frame(build);
        REQUIRE(value == "abc");
        h.input.setKeyDown(devgui::EditKey::Backspace, false);
        h.frame(build);
    }
    SUBCASE("keeps the text on Enter")
    {
        type("abc");
        h.pressKey(devgui::EditKey::Enter, build);
        REQUIRE(deactivated);
        REQUIRE_FALSE(r.active);
        REQUIRE(value == "abc");
        REQUIRE_FALSE(h.context.getInputCapture().keyboard);
    }
    SUBCASE("restores the text on Escape")
    {
        type("abc");
        h.pressKey(devgui::EditKey::Escape, build);
        REQUIRE(deactivated);
        REQUIRE(value.empty());
    }
    SUBCASE("loses focus to a press elsewhere")
    {
        type("abc");
        h.click(100.0f, 300.0f, build);
        REQUIRE(deactivated);
        REQUIRE(value == "abc");
        type("d");
        REQUIRE(value == "abc");
    }
    SUBCASE("does not treat an outside held Enter as a new key when focus returns")
    {
        type("abc");
        h.click(100.0f, 300.0f, build);
        REQUIRE_FALSE(r.active);

        const auto rebuild = [&] {
            const auto& context = h.context;
            const auto needed = context.needsUpdate(h.input);
            if (needed) {
                h.frame(build);
            }
            return needed;
        };

        h.input.setKeyDown(devgui::EditKey::Enter, true);
        REQUIRE(rebuild());
        REQUIRE_FALSE(r.active);
        const auto builtFrames = h.context.getFrameCount();
        h.input.timeSeconds = 1.0;
        REQUIRE_FALSE(rebuild());
        REQUIRE_FALSE(rebuild());
        REQUIRE(h.context.getFrameCount() == builtFrames);

        h.moveTo(400.0f, 16.0f);
        REQUIRE(rebuild());
        h.input.primaryButtonDown = true;
        REQUIRE(rebuild());
        REQUIRE(r.activated);
        REQUIRE(r.active);
        REQUIRE_FALSE(r.deactivated);

        h.input.primaryButtonDown = false;
        REQUIRE(rebuild());
        REQUIRE(r.active);
        h.input.setKeyDown(devgui::EditKey::Enter, false);
        REQUIRE(rebuild());
        REQUIRE(r.active);
        REQUIRE_FALSE(r.deactivated);
        h.input.setKeyDown(devgui::EditKey::Enter, true);
        REQUIRE(rebuild());
        REQUIRE_FALSE(r.active);
        REQUIRE(r.deactivated);
        REQUIRE(value == "abc");
    }
}

TEST_CASE("devgui::inputTextWithSuggestions")
{
    // NOTE: The field spans x = 321 to 792. Its suggestions open 2 pixels below
    // it, so the first one spans y = 34 to 54 and the second y = 58 to 78.
    Harness h;
    constexpr std::array<std::string_view, 4> names = {{"Apple", "Apricot", "Banana", "Grape"}};
    std::string value;
    std::vector<std::string_view> matches;
    devgui::SuggestionResponse r;
    std::optional<std::size_t> chosen;
    const auto build = [&](devgui::UI& ui) {
        // NOTE: The application decides the suggestions from the value.
        matches.clear();
        if (!value.empty()) {
            for (const auto name : names) {
                if (name.find(value) != std::string_view::npos) {
                    matches.push_back(name);
                }
            }
        }
        r = devgui::inputTextWithSuggestions(ui, "search"_key, "Search", value, matches);
        if (r.chosen) {
            chosen = r.chosen;
        }
    };
    const auto type = [&](std::string_view text) {
        h.input.textInput = text;
        h.frame(build);
    };

    h.click(400.0f, 16.0f, build);
    REQUIRE(r.field.active);
    REQUIRE_FALSE(drawsText(h.context.getDrawList(), "Apple"));

    // NOTE: The suggestions follow the text from the frame after it changes,
    // which the widget asks for.
    type("Ap");
    REQUIRE(h.context.needsUpdate(h.input));
    h.frame(build);
    REQUIRE(drawsText(h.context.getDrawList(), "Apple"));
    REQUIRE(drawsText(h.context.getDrawList(), "Apricot"));
    REQUIRE_FALSE(drawsText(h.context.getDrawList(), "Grape"));

    SUBCASE("chooses the highlighted suggestion with Enter")
    {
        h.pressKey(devgui::EditKey::DownArrow, build);
        h.pressKey(devgui::EditKey::DownArrow, build);
        h.pressKey(devgui::EditKey::UpArrow, build);
        h.pressKey(devgui::EditKey::DownArrow, build);
        REQUIRE(value == "Ap");

        h.input.setKeyDown(devgui::EditKey::Enter, true);
        h.frame(build);
        REQUIRE(value == "Apricot");
        REQUIRE(r.chosen == std::optional<std::size_t>{1});
        REQUIRE(r.field.deactivated);
        h.input.setKeyDown(devgui::EditKey::Enter, false);
        h.frame(build);
        REQUIRE_FALSE(r.field.active);
        REQUIRE_FALSE(drawsText(h.context.getDrawList(), "Apple"));
    }
    SUBCASE("keeps the typed text on Enter without a highlight")
    {
        h.pressKey(devgui::EditKey::Enter, build);
        REQUIRE(value == "Ap");
        REQUIRE_FALSE(chosen);
        REQUIRE_FALSE(r.field.active);
    }
    SUBCASE("chooses a clicked suggestion")
    {
        h.click(400.0f, 44.0f, build);
        REQUIRE(value == "Apple");
        REQUIRE(chosen == std::optional<std::size_t>{0});
        REQUIRE_FALSE(r.field.active);
    }
    SUBCASE("closes the list on Escape and opens it again on typing")
    {
        h.pressKey(devgui::EditKey::Escape, build);
        REQUIRE(value == "Ap");
        REQUIRE(r.field.active);
        REQUIRE_FALSE(drawsText(h.context.getDrawList(), "Apple"));

        type("r");
        h.frame(build);
        REQUIRE(value == "Apr");
        REQUIRE(drawsText(h.context.getDrawList(), "Apricot"));
        REQUIRE_FALSE(drawsText(h.context.getDrawList(), "Apple"));
    }
    SUBCASE("keeps the list open while the field is clicked")
    {
        h.click(330.0f, 16.0f, build);
        REQUIRE(r.field.active);
        REQUIRE(drawsText(h.context.getDrawList(), "Apple"));
        REQUIRE_FALSE(chosen);
    }
    SUBCASE("closes the list when a press ends the editing")
    {
        h.click(100.0f, 300.0f, build);
        REQUIRE_FALSE(r.field.active);
        REQUIRE_FALSE(drawsText(h.context.getDrawList(), "Apple"));
        REQUIRE(value == "Ap");
    }
}

TEST_CASE("devgui::inputTextWithSuggestions scrolls to the highlight")
{
    // NOTE: The list shows 8 of the 12 suggestions in 8 * 20 + 7 * 4 + 12 =
    // 200 pixels below the field, from y = 28 to 228. The tenth suggestion
    // starts at y = 34 + 9 * 24 = 250, out of view until the highlight reaches
    // it.
    Harness h;
    constexpr std::array<std::string_view, 12> names = {{
        "m00",
        "m01",
        "m02",
        "m03",
        "m04",
        "m05",
        "m06",
        "m07",
        "m08",
        "m09",
        "m10",
        "m11",
    }};
    std::string value;
    const auto build = [&](devgui::UI& ui) {
        const auto suggestions = value.empty() ? std::span<const std::string_view>{} : std::span<const std::string_view>{names};
        [[maybe_unused]] const auto r = devgui::inputTextWithSuggestions(ui, "search"_key, "Search", value, suggestions);
    };

    h.click(400.0f, 16.0f, build);
    h.input.textInput = "m";
    h.frame(build);
    h.frame(build);
    REQUIRE(drawsText(h.context.getDrawList(), "m07"));
    REQUIRE_FALSE(drawsText(h.context.getDrawList(), "m09"));

    for (i32 i = 0; i < 10; ++i) {
        h.pressKey(devgui::EditKey::DownArrow, build);
    }
    REQUIRE(drawsText(h.context.getDrawList(), "m09"));
    REQUIRE_FALSE(drawsText(h.context.getDrawList(), "m00"));
}

TEST_CASE("devgui::curveEditor")
{
    // NOTE: The 120-pixel graph spans x = 321 to 792 and y = 6 to 126. Its plot
    // is inset by 8 pixels: x = 329 to 784 and y = 14 to 118.
    Harness h;
    std::vector<devgui::CurvePoint> points = {{0.0f, 0.0f}, {1.0f, 1.0f}};
    devgui::Response r;
    const auto build = [&](devgui::UI& ui) {
        r = devgui::curveEditor(ui, "size"_key, "Size", points, devgui::CurveEditorOptions{.minY = 0.0f, .maxY = 1.0f, .height = 120.0f});
    };
    const auto press = [&](f32 x, f32 y, pomdog::f64 time) {
        h.input.timeSeconds = time;
        h.moveTo(x, y);
        h.frame(build);
        h.input.primaryButtonDown = true;
        h.frame(build);
        h.input.primaryButtonDown = false;
        h.frame(build);
    };

    SUBCASE("drags a point between its neighbors")
    {
        h.moveTo(329.0f, 118.0f);
        h.frame(build);
        h.input.primaryButtonDown = true;
        h.frame(build);
        h.moveTo(329.0f + 455.0f * 0.5f, 14.0f + 104.0f * 0.5f);
        h.frame(build);
        REQUIRE(r.changed);
        REQUIRE(points[0].x == doctest::Approx(0.5));
        REQUIRE(points[0].y == doctest::Approx(0.5));

        // NOTE: The point cannot pass its right neighbor at x = 1.
        h.moveTo(900.0f, 14.0f);
        h.frame(build);
        REQUIRE(points[0].x == doctest::Approx(1.0));
        h.input.primaryButtonDown = false;
        h.frame(build);
    }
    SUBCASE("adds a point on a double click")
    {
        press(329.0f + 455.0f * 0.25f, 14.0f + 104.0f * 0.25f, 1.0);
        REQUIRE(points.size() == 2);
        press(329.0f + 455.0f * 0.25f, 14.0f + 104.0f * 0.25f, 1.1);
        REQUIRE(points.size() == 3);
        REQUIRE(points[1].x == doctest::Approx(0.25));
        REQUIRE(points[1].y == doctest::Approx(0.75));
    }
    SUBCASE("removes a point on a double click")
    {
        press(784.0f, 14.0f, 1.0);
        press(784.0f, 14.0f, 1.1);
        REQUIRE(points.size() == 1);

        // NOTE: The last point stays.
        press(329.0f, 118.0f, 2.0);
        press(329.0f, 118.0f, 2.1);
        REQUIRE(points.size() == 1);
    }
}

TEST_CASE("devgui::tooltip")
{
    Harness h;
    std::string_view text = "Short tip";
    const auto build = [&](devgui::UI& ui) {
        const auto r = devgui::button(ui, "ok"_key, "OK");
        devgui::tooltip(ui, r, text);
    };

    h.input.timeSeconds = 10.0;
    h.moveTo(12.0f, 10.0f);
    h.frame(build);
    REQUIRE_FALSE(drawsText(h.context.getDrawList(), "Short tip"));

    SUBCASE("appears after the delay without other input")
    {
        h.input.timeSeconds = 10.4;
        REQUIRE_FALSE(h.context.needsUpdate(h.input));
        h.input.timeSeconds = 10.5;
        REQUIRE(h.context.needsUpdate(h.input));
        h.frame(build);

        const auto& drawList = h.context.getDrawList();
        REQUIRE(drawsText(drawList, "Short tip"));

        // NOTE: The tooltip lies above everything else.
        const auto ranges = drawList.getRanges();
        REQUIRE_FALSE(ranges.empty());
        REQUIRE(ranges.back().depth == std::numeric_limits<u64>::max());
    }
    SUBCASE("wraps long text")
    {
        // NOTE: 320 pixels hold 45 characters.
        text = "The quick brown fox jumps over the lazy dog. The quick brown fox jumps again.";
        h.input.timeSeconds = 11.0;
        h.frame(build);
        const auto& drawList = h.context.getDrawList();
        REQUIRE(drawsText(drawList, "The quick brown fox jumps over the lazy dog."));
        REQUIRE(drawsText(drawList, "The quick brown fox jumps again."));
    }
    SUBCASE("hides when the pointer leaves")
    {
        h.input.timeSeconds = 11.0;
        h.frame(build);
        h.moveTo(500.0f, 500.0f);
        h.frame(build);
        REQUIRE_FALSE(drawsText(h.context.getDrawList(), "Short tip"));
    }
}
