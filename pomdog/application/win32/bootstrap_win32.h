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

class POMDOG_EXPORT Bootstrap final {
public:
    /// Sets the instance handle.
    void setInstance(HINSTANCE hInstance) noexcept;

    /// Sets the WinMain function's `nCmdShow` parameter.
    void setCommandShow(i32 cmdShow) noexcept;

    /// Sets a window's icon.
    void setIcon(HICON icon) noexcept;

    /// Sets a window's small icon.
    void setIconSmall(HICON iconSmall) noexcept;

    /// Sets command-line arguments to pass to GameSetup::configure().
    void setCommandLineArgs(int argc, const char* const* argv) noexcept;

    /// Sets an error event handler to a log stream.
    void onError(std::function<void(std::unique_ptr<Error>&& err)> onError);

    /// Begins running a game loop using a GameSetup.
    void run(std::unique_ptr<GameSetup>&& app);

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
