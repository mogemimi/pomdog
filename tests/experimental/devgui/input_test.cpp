// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/platform.h"
#include "pomdog/experimental/devgui/input.h"
#include "pomdog/input/backends/keyboard_impl.h"
#include "pomdog/input/backends/mouse_impl.h"
#include "pomdog/input/key_state.h"
#include "pomdog/input/keys.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <initializer_list>
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include <emscripten.h>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

namespace devgui = pomdog::devgui;
using pomdog::Keys;
using pomdog::KeyState;
using pomdog::Vector2;

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
        return devgui::makeInput(mouse, &keyboard, nullptr, Vector2{800.0f, 600.0f}, 0.0);
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

    const auto withoutKeyboard = devgui::makeInput(mouse, nullptr, nullptr, Vector2{800.0f, 600.0f}, 0.0);
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
