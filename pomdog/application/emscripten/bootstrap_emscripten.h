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

class POMDOG_EXPORT Bootstrap final {
public:
    /// Sets the target canvas element id (e.g., "#canvas").
    void setTargetCanvas(const std::string& targetCanvas) noexcept;

    /// Sets an error event handler to a log stream.
    void onError(std::function<void(std::unique_ptr<Error>&& err)> onError);

    /// Sets a callback invoked with the GameWindow pointer after the window is created.
    /// Use this to store the window pointer for external callers (e.g., JS-callable C functions).
    void onWindowCreated(std::function<void(unsafe_ptr<GameWindow> window)> callback);

    /// Begins running a game loop using a GameSetup.
    void run(std::unique_ptr<GameSetup>&& gameSetup);

private:
    std::function<void(std::unique_ptr<Error>&& err)> onError_;
    std::function<void(unsafe_ptr<GameWindow> window)> onWindowCreated_;
    std::string targetCanvas_ = "#canvas";
};

} // namespace pomdog::emscripten
