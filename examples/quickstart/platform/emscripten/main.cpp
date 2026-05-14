// Copyright mogemimi. Distributed under the MIT license.

#include "game_setup.h"
#include "pomdog/application/emscripten/bootstrap_emscripten.h"
#include "pomdog/application/window_mode.h"
#include "pomdog/console/console.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/pomdog.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten/console.h>
#include <emscripten/emscripten.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace {

pomdog::unsafe_ptr<pomdog::GameWindow> currentWindow = nullptr;

} // namespace

extern "C" {

void pomdog_enter_full_screen()
{
    if (currentWindow != nullptr) {
        // NOTE: Request browser fullscreen via engine API.
        [[maybe_unused]] auto err = currentWindow->requestWindowMode(pomdog::WindowMode::Fullscreen);
    }
}

void pomdog_enter_soft_full_screen()
{
    if (currentWindow != nullptr) {
        // NOTE: Resize canvas to fill the browser window without entering fullscreen mode.
        [[maybe_unused]] auto err = currentWindow->requestWindowMode(pomdog::WindowMode::BrowserSoftFullscreen);
    }
}

} // extern "C"

int main()
{
    using namespace pomdog;

    pomdog::emscripten::Bootstrap bootstrap = {};
    bootstrap.setTargetCanvas("#canvas");

#if defined(POMDOG_DEBUG_BUILD)
    Log::SetLevel(LogLevel::Internal);
    ScopedConnection connection = Log::Connect([](const LogEntry& entry) {
        pomdog::console::write_line(entry.Message);
    });
#endif

    bootstrap.onError([](std::unique_ptr<Error>&& err) {
        pomdog::console::write_error_line(err->toString());
    });

    bootstrap.onWindowCreated([](pomdog::unsafe_ptr<pomdog::GameWindow> window) {
        currentWindow = window;
    });

    bootstrap.run(quickstart::createGameSetup());

    currentWindow = nullptr;
    return 0;
}
