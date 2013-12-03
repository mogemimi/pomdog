//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_COCOA_GAMEWINDOW_HPP
#define POMDOG_COCOA_GAMEWINDOW_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Application/GameWindow.hpp>
#include <Pomdog/Math/Rectangle.hpp>
#import <Cocoa/Cocoa.h>

@class NSWindow, CocoaOpenGLView;

namespace Pomdog {
namespace Details {
namespace Cocoa {

class CocoaGameWindow final: public GameWindow
{
public:
	explicit CocoaGameWindow(NSWindow* nativeWindow);
	~CocoaGameWindow();

	///@~Japanese
	/// @copydoc GameWindow
	bool GetAllowPlayerResizing() const override;

	///@~Japanese
	/// @copydoc GameWindow
	void SetAllowPlayerResizing(bool allowResizing) override;

	///@~Japanese
	/// @copydoc GameWindow
	std::string GetCaption() const override;

	///@~Japanese
	/// @copydoc GameWindow
	void SetCaption(std::string const& caption) override;

	///@~Japanese
	/// @copydoc GameWindow
	Rectangle GetClientBounds() const override;

	///@~Japanese
	/// @copydoc GameWindow
	void SetClientBounds(Rectangle const& clientBounds) override;
	
	///@~English
	/// @return true if the window is minimized, false otherwise.
	///@~Japanese
	/// @brief ウィンドウが最小化状態かどうかを取得します。
	/// @return ウィンドウが最小化のときは true を、それ以外は false を返します。
	bool IsMinimized() const;
		
private:
	NSWindow* nativeWindow;
	CocoaOpenGLView* openGLView;
};

}// namespace Cocoa
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_COCOAGAMEWINDOW_HPP)
