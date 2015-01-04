//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog.Experimental/UI/MouseHelper.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"

namespace Pomdog {
namespace UI {
namespace {

static LPCTSTR ToStandardCursorID(MouseCursor cursor)
{
	switch (cursor) {
	case MouseCursor::Arrow: return IDC_ARROW;
	case MouseCursor::Text: return IDC_IBEAM;
	case MouseCursor::Link: return IDC_HAND;
	case MouseCursor::ResizeHorizontal: return IDC_SIZEWE;
	case MouseCursor::ResizeVertical: return IDC_SIZENS;
	}
	return IDC_ARROW;
}

}// unnamed namespace
//-----------------------------------------------------------------------
void MouseHelper::ShowMouseCursor()
{
	///@todo Not implemented
	HINSTANCE instance = nullptr;
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

	HCURSOR cursorHandle = LoadCursor(instance, ToStandardCursorID(cursor));
	SetCursor(cursorHandle);
}
//-----------------------------------------------------------------------
}// namespace UI
}// namespace Pomdog
