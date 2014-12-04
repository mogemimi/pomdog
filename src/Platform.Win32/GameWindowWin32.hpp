//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_WIN32_GAMEWINDOWWIN32_2EC902FC_1DBE_4A3C_BCF0_C25FDDBBA5D7_HPP
#define POMDOG_SRC_WIN32_GAMEWINDOWWIN32_2EC902FC_1DBE_4A3C_BCF0_C25FDDBBA5D7_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Win32Prerequisites.hpp"
#include "../Application/SystemEventDispatcher.hpp"
#include <Pomdog/Application/GameWindow.hpp>
#include <memory>

namespace Pomdog {
namespace Details {
namespace RenderSystem {

class PresentationParameters;

}// namespace RenderSystem
namespace Win32 {

class GameWindowWin32 final: public GameWindow {
public:
	GameWindowWin32(HINSTANCE hInstance, int nCmdShow,
		std::shared_ptr<SystemEventDispatcher> const& eventDispatcher,
		Details::RenderSystem::PresentationParameters const& presentationParameters);

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

	///@~English
	/// @return true if the window is minimized, false otherwise.
	///@~Japanese
	/// @brief ウィンドウが最小化状態かどうかを取得します。
	/// @return ウィンドウが最小化のときは true を、それ以外は false を返します。
	bool IsMinimized() const;

	///@~Japanese
	/// @brief ウィンドウを閉じます。
	void Close();

	HWND NativeWindowHandle() const;

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

}// namespace Win32
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_WIN32_GAMEWINDOWWIN32_2EC902FC_1DBE_4A3C_BCF0_C25FDDBBA5D7_HPP)
