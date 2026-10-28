// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/platform.h"
#include "pomdog/experimental/devgui/input.h"
#include "pomdog/input/backends/keyboard_impl.h"
#include "pomdog/input/backends/mouse_impl.h"
#include "pomdog/input/key_state.h"
#include "pomdog/input/keys.h"
#include "pomdog/input/touchscreen.h"
#include "pomdog/math/point2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <initializer_list>
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include <emscripten.h>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace devgui = pomdog::devgui;
using pomdog::Keys;
using pomdog::KeyState;
using pomdog::Point2D;
using pomdog::u8;
using pomdog::Vector2;

namespace {

/// Reports the touch points that a test sets.
class FakeTouchscreen final : public pomdog::Touchscreen {
public:
    std::array<Point2D, 5> positions = {};
    std::array<bool, 5> pressed = {};

    FakeTouchscreen() = default;
    FakeTouchscreen(const FakeTouchscreen&) = delete;
    FakeTouchscreen& operator=(const FakeTouchscreen&) = delete;
    FakeTouchscreen(FakeTouchscreen&&) = delete;
    FakeTouchscreen& operator=(FakeTouchscreen&&) = delete;
    ~FakeTouchscreen() override = default;

    [[nodiscard]] bool
    isMouseSimulationEnabled() const noexcept override
    {
        return false;
    }

    void
    setMouseSimulationEnabled([[maybe_unused]] bool enabled) noexcept override
    {
    }

    [[nodiscard]] u8
    getSupportedTouchCount() const noexcept override
    {
        return static_cast<u8>(positions.size());
    }

    [[nodiscard]] Point2D
    getPosition(u8 touchIndex) const noexcept override
    {
        return positions[touchIndex];
    }

    [[nodiscard]] bool
    isTouchPressed(u8 touchIndex) const noexcept override
    {
        return pressed[touchIndex];
    }

    [[nodiscard]] Point2D
    getPrimaryTouchPosition() const noexcept override
    {
        return positions[0];
    }

    [[nodiscard]] bool
    isPrimaryTouchPressed() const noexcept override
    {
        return pressed[0];
    }
};

} // namespace

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
namespace {

/// Restores the browser or Node navigator after each simulated host platform.
class NavigatorOverride final {
public:
    NavigatorOverride()
    {
        emscripten_run_script(R"(
            Module.pomdogTestNavigator = Object.getOwnPropertyDescriptor(globalThis, "navigator");
            Object.defineProperty(globalThis, "navigator", {configurable: true, writable: true, value: undefined});
        )");
    }

    NavigatorOverride(const NavigatorOverride&) = delete;
    NavigatorOverride& operator=(const NavigatorOverride&) = delete;
    NavigatorOverride(NavigatorOverride&&) = delete;
    NavigatorOverride& operator=(NavigatorOverride&&) = delete;

    ~NavigatorOverride()
    {
        emscripten_run_script(R"(
            if (Module.pomdogTestNavigator) {
                Object.defineProperty(globalThis, "navigator", Module.pomdogTestNavigator);
            } else {
                delete globalThis.navigator;
            }
            delete Module.pomdogTestNavigator;
        )");
    }
};

} // namespace

TEST_CASE("devgui Web shortcut modifier")
{
    const NavigatorOverride navigator;
    bool usesSuper = false;
    SUBCASE("macOS user agent data takes precedence")
    {
        emscripten_run_script("navigator = {userAgentData: {platform: 'macOS'}, platform: 'Win32'};");
        usesSuper = true;
    }
    SUBCASE("macOS without user agent data")
    {
        emscripten_run_script("navigator = {platform: 'MacIntel'};");
        usesSuper = true;
    }
    SUBCASE("iPad hardware keyboard")
    {
        emscripten_run_script("navigator = {platform: 'iPad'};");
        usesSuper = true;
    }
    SUBCASE("Windows user agent data takes precedence")
    {
        emscripten_run_script("navigator = {userAgentData: {platform: 'Windows'}, platform: 'MacIntel'};");
    }
    SUBCASE("Windows without user agent data")
    {
        emscripten_run_script("navigator = {platform: 'Win32'};");
    }
    SUBCASE("Linux")
    {
        emscripten_run_script("navigator = {platform: 'Linux x86_64'};");
    }
    SUBCASE("no navigator")
    {
    }
    REQUIRE(devgui::detail::hostUsesSuperShortcuts() == usesSuper);
}
#endif

TEST_CASE("devgui::makeInput shortcut modifier")
{
#if defined(POMDOG_PLATFORM_MACOSX)
    const bool usesSuper = true;
#elif defined(POMDOG_PLATFORM_EMSCRIPTEN)
    const bool usesSuper = devgui::detail::hostUsesSuperShortcuts();
#else
    const bool usesSuper = false;
#endif

    pomdog::detail::MouseImpl mouse;
    pomdog::detail::KeyboardImpl keyboard;
    const auto readInput = [&]() {
        return devgui::makeInput(mouse, nullptr, &keyboard, nullptr, Vector2{800.0f, 600.0f}, 0.0);
    };

    auto input = readInput();
    REQUIRE(input.shortcutUsesSuper == usesSuper);
    REQUIRE_FALSE(input.shortcutDown);

    for (const auto key : {Keys::LeftSuper, Keys::RightSuper}) {
        for (const auto editKey : {Keys::A, Keys::C, Keys::V, Keys::X}) {
            keyboard.setKey(editKey, KeyState::Down);
        }
        keyboard.setKey(key, KeyState::Down);
        input = readInput();
        REQUIRE(input.shortcutDown == usesSuper);
        REQUIRE(input.isKeyDown(devgui::EditKey::A));
        REQUIRE(input.isKeyDown(devgui::EditKey::C));
        REQUIRE(input.isKeyDown(devgui::EditKey::V));
        REQUIRE(input.isKeyDown(devgui::EditKey::X));
        keyboard.setKey(key, KeyState::Up);
    }
    for (const auto key : {Keys::LeftControl, Keys::RightControl}) {
        keyboard.setKey(key, KeyState::Down);
        input = readInput();
        REQUIRE(input.shortcutDown == !usesSuper);
        keyboard.setKey(key, KeyState::Up);
    }

    const auto withoutKeyboard = devgui::makeInput(mouse, nullptr, nullptr, nullptr, Vector2{800.0f, 600.0f}, 0.0);
    REQUIRE(withoutKeyboard.shortcutUsesSuper == usesSuper);
    REQUIRE_FALSE(withoutKeyboard.shortcutDown);

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    const NavigatorOverride navigator;
    if (usesSuper) {
        emscripten_run_script("navigator = {platform: 'Win32'};");
    }
    else {
        emscripten_run_script("navigator = {platform: 'MacIntel'};");
    }
    REQUIRE(devgui::detail::hostUsesSuperShortcuts() == !usesSuper);
    REQUIRE(readInput().shortcutUsesSuper == usesSuper);
#endif
}

TEST_CASE("devgui::makeInput touchscreen")
{
    pomdog::detail::MouseImpl mouse;
    mouse.setPosition(Point2D{10, 20});
    FakeTouchscreen touchscreen;
    const auto readInput = [&]() {
        return devgui::makeInput(mouse, &touchscreen, nullptr, nullptr, Vector2{800.0f, 600.0f}, 0.0, 2.0f);
    };

    auto input = readInput();
    REQUIRE(input.pointerType == devgui::PointerType::Mouse);
    REQUIRE(input.touchCount == 0);
    REQUIRE(input.pointerPosition == Vector2{5.0f, 10.0f});
    REQUIRE_FALSE(input.pointerPresent);
    REQUIRE_FALSE(input.primaryButtonDown);

    // NOTE: Fingers take the pointer from the mouse, at their center, and
    // the UI scale divides it like the mouse position.
    touchscreen.positions[1] = Point2D{100, 200};
    touchscreen.pressed[1] = true;
    input = readInput();
    REQUIRE(input.pointerType == devgui::PointerType::Touch);
    REQUIRE(input.touchCount == 1);
    REQUIRE(input.pointerPosition == Vector2{50.0f, 100.0f});
    REQUIRE(input.pointerPresent);
    REQUIRE(input.primaryButtonDown);

    touchscreen.positions[3] = Point2D{300, 400};
    touchscreen.pressed[3] = true;
    input = readInput();
    REQUIRE(input.touchCount == 2);
    REQUIRE(input.pointerPosition == Vector2{100.0f, 150.0f});

    touchscreen.pressed[1] = false;
    touchscreen.pressed[3] = false;
    input = readInput();
    REQUIRE(input.pointerType == devgui::PointerType::Mouse);
    REQUIRE(input.touchCount == 0);
    REQUIRE_FALSE(input.primaryButtonDown);
}
