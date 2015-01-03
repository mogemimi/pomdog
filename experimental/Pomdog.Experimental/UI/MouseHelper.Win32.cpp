//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "MouseHelper.Win32.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"

namespace Pomdog {
namespace UI {
//-----------------------------------------------------------------------
void MouseHelper::ShowMouseCursor()
{
	HCURSOR cursor = LoadCursor(instance, IDC_ARROW);
	SetCursor(cursor);
}
//-----------------------------------------------------------------------
void MouseHelper::HideMouseCursor()
{
	SetCursor(nullptr);
}
//-----------------------------------------------------------------------
void MouseHelper::SetMouseCursor(MouseCursor cursor)
{
	///@todo Not implemented
	HINSTANCE instance = nullptr;

	switch (cursor) {
	case MouseCursor::Arrow: {
		HCURSOR cursor = LoadCursor(instance, IDC_ARROW);
		SetCursor(cursor);
		break;
	}
	case MouseCursor::Text: {
		HCURSOR cursor = LoadCursor(instance, IDC_IBEAM);
		SetCursor(cursor);
		break;
	}
	case MouseCursor::Link: {
		HCURSOR cursor = LoadCursor(instance, IDC_HAND);
		SetCursor(cursor);
		break;
	}
	case MouseCursor::ResizeHorizontal: {
		HCURSOR cursor = LoadCursor(instance, IDC_SIZEWE);
		SetCursor(cursor);
		break;
	}
	case MouseCursor::ResizeVertical: {
		HCURSOR cursor = LoadCursor(instance, IDC_SIZENS);
		SetCursor(cursor);
		break;
	}
	}
}
//-----------------------------------------------------------------------
}// namespace UI
}// namespace Pomdog
