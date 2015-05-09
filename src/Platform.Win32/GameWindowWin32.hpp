// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMEWINDOWWIN32_2EC902FC_HPP
#define POMDOG_GAMEWINDOWWIN32_2EC902FC_HPP

#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include "Pomdog/Application/GameWindow.hpp"
#include "Pomdog/Signals/EventQueue.hpp"
#include <memory>

namespace Pomdog {

struct PresentationParameters;

namespace Detail {
namespace Win32 {

class GameWindowWin32 final: public GameWindow {
public:
    GameWindowWin32(HINSTANCE hInstance, int nCmdShow,
        HICON icon, HICON iconSmall, bool useOpenGL,
        std::shared_ptr<EventQueue> const& eventQueue,
        PresentationParameters const& presentationParameters);

    ~GameWindowWin32();

    bool AllowPlayerResizing() const override;

    void AllowPlayerResizing(bool allowResizing) override;

    std::string Title() const override;

    void Title(std::string const& title) override;

    Rectangle ClientBounds() const override;

    void ClientBounds(Rectangle const& clientBounds) override;

    bool IsMouseCursorVisible() const override;

    void IsMouseCursorVisible(bool visible) override;

    void SetMouseCursor(MouseCursor cursor) override;

    ///@return true if the window is minimized, false otherwise.
    bool IsMinimized() const;

    void Close();

    HWND NativeWindowHandle() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Win32
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_GAMEWINDOWWIN32_2EC902FC_HPP
