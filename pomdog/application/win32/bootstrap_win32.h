// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/basic/win32/windows_headers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class GameSetup;
} // namespace pomdog

namespace pomdog::win32 {

/// Bootstrap is the Windows entry point of a Pomdog application.
///
/// Call it from WinMain: set the instance handle and any optional settings,
/// then call run() with a GameSetup. The startup sequence is documented on
/// GameSetup.
class POMDOG_EXPORT Bootstrap final {
public:
    /// Sets the application instance handle. Required; pass the `hInstance`
    /// argument of WinMain.
    void setInstance(HINSTANCE hInstance) noexcept;

    /// Sets the initial window show state; pass the `nCmdShow` argument of
    /// WinMain.
    void setCommandShow(i32 cmdShow) noexcept;

    /// Sets the window icon. Optional.
    void setIcon(HICON icon) noexcept;

    /// Sets the small window icon. Optional.
    void setIconSmall(HICON iconSmall) noexcept;

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
    HINSTANCE hInstance_ = nullptr;
    i32 cmdShow_ = SW_SHOWDEFAULT;
    HICON icon_ = nullptr;
    HICON iconSmall_ = nullptr;
    int argc_ = 0;
    const char* const* argv_ = nullptr;
};

} // namespace pomdog::win32
