// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMEWINDOWCOCOA_BA8D88CA_87F8_46A5_8F32_7FA2C73F58B1_HPP
#define POMDOG_GAMEWINDOWCOCOA_BA8D88CA_87F8_46A5_8F32_7FA2C73F58B1_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "../Application/SystemEventDispatcher.hpp"
#include "Pomdog/Application/GameWindow.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#import <Cocoa/Cocoa.h>

@class NSWindow;
@class CocoaOpenGLView;
@class CocoaWindowDelegate;
@class CocoaGameViewDelegate;

namespace Pomdog {
namespace Detail {
namespace Cocoa {

class OpenGLContextCocoa;
class MouseCocoa;

class GameWindowCocoa final: public GameWindow {
public:
	GameWindowCocoa(NSWindow* window, std::shared_ptr<SystemEventDispatcher> const& eventDispatcher);
	~GameWindowCocoa();

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

	void ResetGLContext(std::shared_ptr<OpenGLContextCocoa> const& context);

	void ResetGLContext();

	void SetRenderCallbackOnLiveResizing(std::function<void()> const& callback);

	void SetRenderCallbackOnLiveResizing();

	void BindToDelegate(std::shared_ptr<MouseCocoa> mouse);

private:
	std::shared_ptr<OpenGLContextCocoa> openGLContext;
	NSWindow* nativeWindow;
	CocoaOpenGLView* openGLView;
	CocoaWindowDelegate* windowDelegate;
	CocoaGameViewDelegate* viewDelegate;
	bool isMouseCursorVisible;
};

}// namespace Cocoa
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_GAMEWINDOWCOCOA_BA8D88CA_87F8_46A5_8F32_7FA2C73F58B1_HPP)
