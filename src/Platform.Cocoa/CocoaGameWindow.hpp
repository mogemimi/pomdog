//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_COCOA_GAMEWINDOW_BA8D88CA_87F8_46A5_8F32_7FA2C73F58B1_HPP
#define POMDOG_SRC_COCOA_GAMEWINDOW_BA8D88CA_87F8_46A5_8F32_7FA2C73F58B1_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#import <Cocoa/Cocoa.h>
#include <Pomdog/Application/GameWindow.hpp>
#include <Pomdog/Math/Rectangle.hpp>
#include "../Application/SystemEventDispatcher.hpp"

@class NSWindow, CocoaOpenGLView;
@class CocoaWindowDelegate;
@class CocoaOpenGLViewDelegate;

namespace Pomdog {
namespace Details {
namespace Cocoa {

class CocoaOpenGLContext;
class CocoaMouse;

class CocoaGameWindow final: public GameWindow {
public:
	CocoaGameWindow(NSWindow* window, std::shared_ptr<SystemEventDispatcher> eventDispatcher);
	~CocoaGameWindow();

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
	
	void ResetGLContext(std::shared_ptr<CocoaOpenGLContext> context);
	void ResetGLContext();

	///@~Japanese
	/// @brief ウィンドウを閉じます。
	void Close();
	
	///@~Japanese
	/// @brief マウスをビューデリゲートに接続します。
	void BindToDelegate(std::shared_ptr<CocoaMouse> mouse);

private:
	std::shared_ptr<CocoaOpenGLContext> openGLContext;
	NSWindow* nativeWindow;
	CocoaOpenGLView* openGLView;
	CocoaWindowDelegate* windowDelegate;
	CocoaOpenGLViewDelegate* viewDelegate;
};

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_COCOAGAMEWINDOW_BA8D88CA_87F8_46A5_8F32_7FA2C73F58B1_HPP)
