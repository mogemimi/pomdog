// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/text_input/platform_text_input.h"
#include "pomdog/application/game_host.h"
#include "pomdog/basic/platform.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

#if defined(POMDOG_PLATFORM_WIN32)
#include "pomdog/application/win32/game_window_win32.h"
#include "pomdog/experimental/text_input/win32/text_input_service_win32.h"
#elif defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include "pomdog/application/emscripten/game_window_emscripten.h"
#include "pomdog/experimental/text_input/emscripten/text_input_service_emscripten.h"
#elif defined(POMDOG_PLATFORM_LINUX)
#include "pomdog/application/x11/game_window_x11.h"
#include "pomdog/experimental/text_input/x11/text_input_service_x11.h"
#else
#include "pomdog/experimental/text_input/text_input_service_keyboard.h"
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::textinput {

std::tuple<std::shared_ptr<TextInputService>, std::unique_ptr<Error>>
createTextInputService(const std::shared_ptr<GameHost>& gameHost)
{
    POMDOG_ASSERT(gameHost != nullptr);

#if defined(POMDOG_PLATFORM_WIN32)
    auto window = std::dynamic_pointer_cast<pomdog::detail::win32::GameWindowWin32>(gameHost->getWindow());
    if (window == nullptr) {
        return {nullptr, errors::make("the game host has no Win32 window")};
    }
    auto service = std::make_shared<detail::win32::TextInputServiceWin32>();
    if (auto err = service->initialize(window, window->getNativeWindowHandle()); err != nullptr) {
        return {nullptr, errors::wrap(std::move(err), "TextInputServiceWin32::initialize() failed")};
    }
    return {std::move(service), nullptr};
#elif defined(POMDOG_PLATFORM_EMSCRIPTEN)
    auto window = std::dynamic_pointer_cast<pomdog::detail::emscripten::GameWindowEmscripten>(gameHost->getWindow());
    if (window == nullptr) {
        return {nullptr, errors::make("the game host has no canvas window")};
    }
    auto service = std::make_shared<detail::emscripten::TextInputServiceEmscripten>();
    if (auto err = service->initialize(window->getTargetCanvas()); err != nullptr) {
        return {nullptr, errors::wrap(std::move(err), "TextInputServiceEmscripten::initialize() failed")};
    }
    return {std::move(service), nullptr};
#elif defined(POMDOG_PLATFORM_LINUX)
    auto window = std::dynamic_pointer_cast<pomdog::detail::x11::GameWindowX11>(gameHost->getWindow());
    if (window == nullptr) {
        return {nullptr, errors::make("the game host has no X11 window")};
    }
    auto service = std::make_shared<detail::x11::TextInputServiceX11>();
    if (auto err = service->initialize(window); err != nullptr) {
        return {nullptr, errors::wrap(std::move(err), "TextInputServiceX11::initialize() failed")};
    }
    return {std::move(service), nullptr};
#else
    auto keyboard = gameHost->getKeyboard();
    if (keyboard == nullptr) {
        return {nullptr, errors::make("the game host has no keyboard")};
    }
    return {std::make_shared<TextInputServiceKeyboard>(std::move(keyboard)), nullptr};
#endif
}

} // namespace pomdog::textinput
