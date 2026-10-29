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

    /// Sets the handler invoked when the application cannot continue: startup
    /// fails, or the game loop stops on an error it cannot recover from, such
    /// as a lost graphics device. Errors are reported through this handler
    /// because run() does not return a value.
    ///
    /// The handler is the final notice of the failure. Recoverable conditions,
    /// such as an unsupported present mode that falls back to a supported one,
    /// are not reported.
    void onError(std::function<void(std::unique_ptr<Error>&& err)> onError);

    /// Sets the handler that receives diagnostics. Optional.
    ///
    /// A diagnostic reports a failure that a subsystem already handled while
    /// keeping a safe state, such as a gamepad that could not be opened and is
    /// skipped. The application keeps running and has nothing to recover; the
    /// handler only decides where to report it. It is called on the main
    /// thread between frames.
    void onDiagnostic(std::function<void(std::unique_ptr<Error>&& diagnostic)> onDiagnostic);

    /// Runs the game application and returns after the game loop has
    /// finished or the application has failed.
    void run(std::unique_ptr<GameSetup>&& gameSetup);

private:
    std::function<void(std::unique_ptr<Error>&& err)> onError_;
    std::function<void(std::unique_ptr<Error>&& diagnostic)> onDiagnostic_;
    HINSTANCE hInstance_ = nullptr;
    i32 cmdShow_ = SW_SHOWDEFAULT;
    HICON icon_ = nullptr;
    HICON iconSmall_ = nullptr;
    int argc_ = 0;
    const char* const* argv_ = nullptr;
};

} // namespace pomdog::win32
