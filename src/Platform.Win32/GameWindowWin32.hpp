// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SRC_WIN32_GAMEWINDOWWIN32_2EC902FC_1DBE_4A3C_BCF0_C25FDDBBA5D7_HPP
#define POMDOG_SRC_WIN32_GAMEWINDOWWIN32_2EC902FC_1DBE_4A3C_BCF0_C25FDDBBA5D7_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "../Application/SystemEventDispatcher.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include "Pomdog/Application/GameWindow.hpp"
#include <memory>

namespace Pomdog {

class PresentationParameters;

namespace Detail {
namespace Win32 {

class GameWindowWin32 final: public GameWindow {
public:
    GameWindowWin32(HINSTANCE hInstance, int nCmdShow,
        HICON icon, HICON iconSmall, bool useOpenGL,
        std::shared_ptr<SystemEventDispatcher> const& eventDispatcher,
        PresentationParameters const& presentationParameters);

    ~GameWindowWin32();

    ///@copydoc GameWindow
    bool AllowPlayerResizing() const override;

    ///@copydoc GameWindow
    void AllowPlayerResizing(bool allowResizing) override;

    ///@copydoc GameWindow
    std::string Title() const override;

    ///@copydoc GameWindow
    void Title(std::string const& title) override;

    ///@copydoc GameWindow
    Rectangle ClientBounds() const override;

    ///@copydoc GameWindow
    void ClientBounds(Rectangle const& clientBounds) override;

    ///@copydoc GameWindow
    bool IsMouseCursorVisible() const override;

    ///@copydoc GameWindow
    void IsMouseCursorVisible(bool visible) override;

    ///@copydoc GameWindow
    void SetMouseCursor(MouseCursor cursor) override;

    ///@return true if the window is minimized, false otherwise.
    bool IsMinimized() const;

    void Close();

    HWND NativeWindowHandle() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}// namespace Win32
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_WIN32_GAMEWINDOWWIN32_2EC902FC_1DBE_4A3C_BCF0_C25FDDBBA5D7_HPP)
