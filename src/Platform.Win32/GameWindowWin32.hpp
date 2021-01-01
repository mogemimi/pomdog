// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/GameWindow.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include "Pomdog/Signals/EventQueue.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
class Error;
struct PresentationParameters;
} // namespace Pomdog

namespace Pomdog::Detail {
class SystemEvent;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Win32 {

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

} // namespace Pomdog::Detail::Win32
