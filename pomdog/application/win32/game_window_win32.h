// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/game_window.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/platform/win32/prerequisites_win32.h"
#include "pomdog/signals/event_queue.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
struct PresentationParameters;
} // namespace pomdog

namespace pomdog::detail {
class SystemEvent;
} // namespace pomdog::detail

namespace pomdog::detail::win32 {

class GameWindowWin32 final : public GameWindow {
public:
    GameWindowWin32();

    ~GameWindowWin32();

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        HINSTANCE hInstance,
        int nCmdShow,
        HICON icon,
        HICON iconSmall,
        bool useOpenGL,
        const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
        const PresentationParameters& presentationParameters) noexcept;

    bool GetAllowUserResizing() const override;

    void SetAllowUserResizing(bool allowResizing) override;

    std::string GetTitle() const override;

    void SetTitle(const std::string& title) override;

    Rectangle GetClientBounds() const override;

    void SetClientBounds(const Rectangle& clientBounds) override;

    bool IsMouseCursorVisible() const override;

    void SetMouseCursorVisible(bool visible) override;

    void SetMouseCursor(MouseCursor cursor) override;

    ///@return true if the window is minimized, false otherwise.
    bool IsMinimized() const;

    void Close();

    HWND GetNativeWindowHandle() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog::detail::win32
