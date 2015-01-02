//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "MouseHelper.hpp"
#import <Cocoa/Cocoa.h>

namespace Pomdog {
namespace UI {
//-----------------------------------------------------------------------
void MouseHelper::ShowMouseCursor()
{
	[NSCursor unhide];
}
//-----------------------------------------------------------------------
void MouseHelper::HideMouseCursor()
{
	[NSCursor hide];
}
//-----------------------------------------------------------------------
void MouseHelper::SetMouseCursor(MouseCursor cursor)
{
	switch (cursor) {
	case MouseCursor::Arrow: {
		[[NSCursor arrowCursor] set];
		break;
	}
	case MouseCursor::Text: {
		[[NSCursor IBeamCursor] set];
		break;
	}
	case MouseCursor::Link: {
		[[NSCursor pointingHandCursor] set];
		break;
	}
	case MouseCursor::ResizeHorizontal: {
		[[NSCursor resizeLeftRightCursor] set];
		break;
	}
	case MouseCursor::ResizeVertical: {
		[[NSCursor resizeUpDownCursor] set];
		break;
	}
	}
}
//-----------------------------------------------------------------------
}// namespace UI
}// namespace Pomdog
