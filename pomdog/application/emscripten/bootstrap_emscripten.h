// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class GameSetup;
} // namespace pomdog

namespace pomdog::emscripten {

class POMDOG_EXPORT Bootstrap final {
public:
    /// Sets the target canvas element id (e.g., "#canvas").
    void setTargetCanvas(const std::string& targetCanvas) noexcept;

    /// Sets an error event handler to a log stream.
    void onError(std::function<void(std::unique_ptr<Error>&& err)> onError);

    /// Begins running a game loop using a GameSetup.
    void run(std::unique_ptr<GameSetup>&& gameSetup);

private:
    std::function<void(std::unique_ptr<Error>&& err)> onError_;
    std::string targetCanvas_ = "#canvas";
};

} // namespace pomdog::emscripten
