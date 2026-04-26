// Copyright mogemimi. Distributed under the MIT license.

#include "game_setup.h"
#include "pomdog/application/emscripten/bootstrap_emscripten.h"
#include "pomdog/pomdog.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten/console.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace {

pomdog::GameWindow* currentWindow = nullptr;

} // namespace

extern "C" {

void pomdog_enter_full_screen()
{
    if (currentWindow != nullptr) {
        emscripten_request_fullscreen("#canvas", EM_TRUE);
    }
}

void pomdog_enter_soft_full_screen()
{
    if (currentWindow != nullptr) {
        EmscriptenFullscreenStrategy strategy{};
        strategy.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_STRETCH;
        strategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
        strategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
        emscripten_enter_soft_fullscreen("#canvas", &strategy);
    }
}

} // extern "C"

int main()
{
    using namespace pomdog;

    pomdog::emscripten::Bootstrap bootstrap = {};
    bootstrap.setTargetCanvas("#canvas");

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    Log::SetLevel(LogLevel::Internal);
    ScopedConnection connection = Log::Connect([](LogEntry const& entry) {
        emscripten_console_log(entry.Message.c_str());
    });
#endif

    bootstrap.onError([](std::unique_ptr<Error>&& err) {
        emscripten_console_error(err->toString().c_str());
    });

    bootstrap.run(pong::createGameSetup());

    currentWindow = nullptr;
    return 0;
}
