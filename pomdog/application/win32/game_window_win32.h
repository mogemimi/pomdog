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
} // namespace pomdog

namespace pomdog::gpu {
struct PresentationParameters;
} // namespace pomdog::gpu

namespace pomdog::detail {
class SystemEvent;
} // namespace pomdog::detail

namespace pomdog::detail::win32 {

class GameWindowWin32 final : public GameWindow {
public:
    GameWindowWin32();

    ~GameWindowWin32();

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        HINSTANCE hInstance,
        int nCmdShow,
        HICON icon,
        HICON iconSmall,
        bool useOpenGL,
        const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
        const gpu::PresentationParameters& presentationParameters) noexcept;

    bool getAllowUserResizing() const override;

    void setAllowUserResizing(bool allowResizing) override;

    std::string getTitle() const override;

    void setTitle(const std::string& title) override;

    Rectangle getClientBounds() const override;

    void setClientBounds(const Rectangle& clientBounds) override;

    bool isMouseCursorVisible() const override;

    void setMouseCursorVisible(bool visible) override;

    void setMouseCursor(MouseCursor cursor) override;

    ///@return true if the window is minimized, false otherwise.
    bool isMinimized() const;

    void close();

    HWND getNativeWindowHandle() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace pomdog::detail::win32
