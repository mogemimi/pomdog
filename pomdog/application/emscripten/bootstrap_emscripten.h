// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/memory/unsafe_ptr.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class GameSetup;
class GameWindow;
} // namespace pomdog

namespace pomdog::emscripten {

/// Bootstrap is the web (Emscripten) entry point of a Pomdog application.
///
/// Call it from main(): optionally set the target canvas and the launch
/// arguments, then call run() with a GameSetup. The startup sequence is
/// documented on GameSetup.
class POMDOG_EXPORT Bootstrap final {
public:
    /// Sets the CSS selector of the canvas element the game renders into.
    void setTargetCanvas(const std::string& targetCanvas) noexcept;

    /// Sets the launch arguments passed to GameSetup::configure(), including
    /// argv[0]. Optional; on the web, arguments typically reach main()
    /// through `Module.arguments`.
    void setCommandLineArgs(int argc, const char* const* argv) noexcept;

    /// Sets the handler invoked when startup fails. Errors are reported
    /// through this handler because run() does not return a value.
    void onError(std::function<void(std::unique_ptr<Error>&& err)> onError);

    /// Sets a callback that receives the GameWindow right after the window
    /// is created, before the game starts. Intended for exposing the window
    /// to external callers such as JS-callable C functions; the pointer
    /// stays valid while the game is running.
    void onWindowCreated(std::function<void(unsafe_ptr<GameWindow> window)> callback);

    /// Starts the game application and hands the main loop to the browser.
    /// On success this function does not return; on a startup failure it
    /// reports the error through the onError() handler and returns.
    void run(std::unique_ptr<GameSetup>&& gameSetup);

private:
    std::function<void(std::unique_ptr<Error>&& err)> onError_;
    std::function<void(unsafe_ptr<GameWindow> window)> onWindowCreated_;
    std::string targetCanvas_ = "#canvas";
    int argc_ = 0;
    const char* const* argv_ = nullptr;
};

} // namespace pomdog::emscripten
