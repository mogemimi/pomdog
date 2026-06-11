// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class GameSetup;
} // namespace pomdog

namespace pomdog::x11 {

/// Bootstrap is the Linux (X11) entry point of a Pomdog application.
///
/// Call it from main(): optionally set the command-line arguments, then call
/// run() with a GameSetup. The startup sequence is documented on GameSetup.
class POMDOG_EXPORT Bootstrap final {
public:
    /// Sets the command-line arguments passed to GameSetup::configure(),
    /// including argv[0]. Optional.
    void setCommandLineArgs(int argc, const char* const* argv) noexcept;

    /// Sets the handler invoked when startup fails. Errors are reported
    /// through this handler because run() does not return a value.
    void onError(std::function<void(std::unique_ptr<Error>&& err)> onError);

    /// Runs the game application and returns after the game loop has
    /// finished or startup has failed.
    void run(std::unique_ptr<GameSetup>&& gameSetup);

private:
    std::function<void(std::unique_ptr<Error>&& err)> onError_;
    int argc_ = 0;
    const char* const* argv_ = nullptr;
};

} // namespace pomdog::x11
