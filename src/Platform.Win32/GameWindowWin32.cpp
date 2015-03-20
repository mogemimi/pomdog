// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "GameWindowWin32.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Application/MouseCursor.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <objbase.h>
#include <string>

namespace Pomdog {
namespace Detail {
namespace Win32 {
namespace {

template <typename T>
static LPSTR MakeIntegerResource(T && resource)
{
#pragma warning(push)
#pragma warning(disable:4302)
	return MAKEINTRESOURCE(std::forward<T>(resource));
#pragma warning(pop)
}
//-----------------------------------------------------------------------
static LPCTSTR ToStandardCursorID(MouseCursor cursor)
{
	switch (cursor) {
	case MouseCursor::Arrow: return IDC_ARROW;
	case MouseCursor::IBeam: return IDC_IBEAM;
	case MouseCursor::PointingHand: return IDC_HAND;
	case MouseCursor::ResizeHorizontal: return IDC_SIZEWE;
	case MouseCursor::ResizeVertical: return IDC_SIZENS;
	}
	return IDC_ARROW;
}

}// unnamed namespace
//-----------------------------------------------------------------------
class GameWindowWin32::Impl {
public:
	Impl(HINSTANCE hInstance, int nCmdShow,
		HICON icon, HICON iconSmall, bool useOpenGL,
		std::shared_ptr<SystemEventDispatcher> const& eventDispatcher,
		PresentationParameters const& presentationParameters);

	~Impl();

	void SetAllowPlayerResizing(bool allowResizing);

	void SetTitle(std::string const& caption);

	void SetClientBounds(Rectangle const& clientBounds);

	void SetMouseCursorVisible(bool visible);

	void SetMouseCursor(MouseCursor cursor);

private:
	static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	std::shared_ptr<SystemEventDispatcher> eventDispatcher;
	std::string title;
	Rectangle clientBounds;
	Optional<HCURSOR> gameCursor;
	HINSTANCE instanceHandle;
	HWND windowHandle;
	bool allowPlayerResizing;
	bool isFullScreen;
	bool isMouseCursorVisible;
};
//-----------------------------------------------------------------------
GameWindowWin32::Impl::Impl(HINSTANCE hInstance, int nCmdShow,
	HICON icon, HICON iconSmall, bool useOpenGL,
	std::shared_ptr<SystemEventDispatcher> const& eventDispatcherIn,
	PresentationParameters const& presentationParameters)
	: eventDispatcher(eventDispatcherIn)
	, title("Game")
	, clientBounds(0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight)
	, instanceHandle(hInstance)
	, windowHandle(nullptr)
	, allowPlayerResizing(false)
	, isFullScreen(presentationParameters.IsFullScreen)
	, isMouseCursorVisible(true)
{
	POMDOG_ASSERT(clientBounds.Width > 0);
	POMDOG_ASSERT(clientBounds.Height > 0);

	DWORD windowStyle = 0;
	DWORD windowStyleEx = 0;
	LONG adjustedWidth = static_cast<LONG>(clientBounds.Width);
	LONG adjustedHeight = static_cast<LONG>(clientBounds.Height);

	if (useOpenGL) {
		windowStyle |= WS_CLIPCHILDREN;
		windowStyle |= WS_CLIPSIBLINGS;
	}

	if (isFullScreen)
	{
		windowStyleEx |= WS_EX_TOPMOST;
		windowStyle |= WS_POPUP;
		clientBounds.X = 0;
		clientBounds.Y = 0;
	}
	else
	{
		DWORD const fixedWindowStyle(WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
		windowStyle |= fixedWindowStyle;

		RECT windowRect = { 0, 0, static_cast<LONG>(clientBounds.Width), static_cast<LONG>(clientBounds.Height) };
		AdjustWindowRect(&windowRect, windowStyle, FALSE);

		adjustedWidth = windowRect.right - windowRect.left;
		adjustedHeight = windowRect.bottom - windowRect.top;
	}

	constexpr auto windowName = "Pomdog.GameWindowWin32";

	UINT windowClassStyle = (CS_HREDRAW | CS_VREDRAW);

	if (useOpenGL) {
		windowClassStyle |= CS_OWNDC;
	}

	if (icon == nullptr) {
		icon = LoadIcon(instanceHandle, MakeIntegerResource(IDI_APPLICATION));
	}

	if (iconSmall == nullptr) {
		iconSmall = LoadIcon(instanceHandle, MakeIntegerResource(IDI_APPLICATION));
	}

	WNDCLASSEX wcex = {
		sizeof(WNDCLASSEX),
		windowClassStyle,
		Impl::WindowProcedure,
		0, 0,
		instanceHandle,
		icon,
		LoadCursor(nullptr, IDC_ARROW),
		static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)),
		nullptr,
		windowName,
		iconSmall
	};

	if (0 == ::RegisterClassEx(&wcex))
	{
		///@todo Not implemented
		//POMDOG_THROW_EXCEPTION(ExceptionCode::RuntimeAssertionFailed,
		//	"Call to RegisterClassEx failed!",
		//	"Win32GameWindow::Impl::Initialize");
	}

	windowHandle = CreateWindowEx(windowStyleEx, wcex.lpszClassName, title.c_str(), windowStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, adjustedWidth, adjustedHeight, nullptr, nullptr, instanceHandle, nullptr);

	if (nullptr == windowHandle)
	{
		///@todo Not implemented
		//POMDOG_THROW_EXCEPTION(ExceptionCode::RuntimeAssertionFailed,
		//	"Call to CreateWindow failed!",
		//	"Win32GameWindow::Impl::Initialize");
	}

	///@note See http://msdn.microsoft.com/ja-jp/library/ff485844(v=vs.85).aspx
	//if (FAILED(::CoInitialize(0)))
	if (FAILED(::CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE)))
	{
		///@todo Not implemented
		//POMDOG_THROW_EXCEPTION(ExceptionCode::RuntimeAssertionFailed,
		//	"Failed to CoInitializeEX",
		//	"Win32GameWindow::Impl::Initialize");
	}

	if (nCmdShow == SW_MAXIMIZE) {
		::ShowWindow(windowHandle, SW_RESTORE);
	}
	else {
		::ShowWindow(windowHandle, nCmdShow);
	}

	{
		POINT point = { 0, 0 };
		if (0 != ::ClientToScreen(windowHandle, &point))
		{
			clientBounds.X = static_cast<std::int32_t>(point.x);
			clientBounds.Y = static_cast<std::int32_t>(point.y);
		}
	}

	::SetWindowLong(windowHandle, GWL_USERDATA, reinterpret_cast<LONG_PTR>(this));
}
//-----------------------------------------------------------------------
GameWindowWin32::Impl::~Impl()
{
	if (nullptr != windowHandle)
	{
		::DestroyWindow(windowHandle);
		::SetWindowLong(windowHandle, GWL_USERDATA, 0);
		windowHandle = nullptr;

		::CoUninitialize();
	}

	instanceHandle = nullptr;
}
//-----------------------------------------------------------------------
void GameWindowWin32::Impl::SetAllowPlayerResizing(bool allowResizingIn)
{
	POMDOG_ASSERT(nullptr != windowHandle);

	if (isFullScreen) {
		return;
	}

	LONG_PTR windowStyle = ::GetWindowLongPtr(windowHandle, GWL_STYLE);
	if (0 == windowStyle) {
		return;
	}

	if (allowResizingIn) {
		windowStyle |= WS_THICKFRAME;
	}
	else {
		windowStyle &= ~WS_THICKFRAME;
	}

	if (0 == ::SetWindowLongPtr(windowHandle, GWL_STYLE, windowStyle))
	{
		///@todo Not implemented
		return;
	}

	if (0 == ::SetWindowPos(windowHandle, 0, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_FRAMECHANGED))
	{
		///@todo Not implemented
		return;
	}

	allowPlayerResizing = allowResizingIn;
}
//-----------------------------------------------------------------------
void GameWindowWin32::Impl::SetTitle(std::string const& titleIn)
{
	POMDOG_ASSERT(nullptr != windowHandle);

	if (0 == ::SetWindowText(windowHandle, titleIn.c_str())) {
		///@todo Not implemented
		return;
	}

	this->title = titleIn;
}
//-----------------------------------------------------------------------
void GameWindowWin32::Impl::SetClientBounds(Rectangle const& clientBoundsIn)
{
	POMDOG_ASSERT(nullptr != windowHandle);

	if (isFullScreen) {
		return;
	}

	DWORD const dwStyle = static_cast<DWORD>(::GetWindowLong(windowHandle, GWL_STYLE));

	RECT windowRect = { 0, 0, static_cast<LONG>(clientBoundsIn.Width), static_cast<LONG>(clientBoundsIn.Height) };

	AdjustWindowRect(&windowRect, dwStyle, FALSE);

	int const adjustedWidth = static_cast<int>(windowRect.right - windowRect.left);
	int const adjustedHeight = static_cast<int>(windowRect.bottom - windowRect.top);
	UINT const flags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE;

	if (0 == ::SetWindowPos(windowHandle, 0, 0, 0, adjustedWidth, adjustedHeight, flags)) {
		///@todo Not implemented
		return;
	}

	clientBounds.Width = clientBoundsIn.Width;
	clientBounds.Height = clientBoundsIn.Height;
}
//-----------------------------------------------------------------------
void GameWindowWin32::Impl::SetMouseCursorVisible(bool visibleIn)
{
	isMouseCursorVisible = visibleIn;

	if (isMouseCursorVisible) {
		if (gameCursor) {
			::SetCursor(*gameCursor);
		}
		else {
			auto nativeCursor = LoadCursor(nullptr, ToStandardCursorID(MouseCursor::Arrow));
			::SetCursor(nativeCursor);
		}
	}
	else {
		::SetCursor(nullptr);
	}
}
//-----------------------------------------------------------------------
void GameWindowWin32::Impl::SetMouseCursor(MouseCursor cursorIn)
{
	gameCursor = LoadCursor(nullptr, ToStandardCursorID(cursorIn));

	if (isMouseCursorVisible) {
		::SetCursor(*gameCursor);
	}
}
//-----------------------------------------------------------------------
LRESULT CALLBACK GameWindowWin32::Impl::WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto window = reinterpret_cast<GameWindowWin32::Impl*>(::GetWindowLong(hWnd, GWL_USERDATA));

	POMDOG_ASSERT(WM_QUIT != msg);

	switch (msg) {
	case WM_CLOSE: {
		if (window) {
			window->eventDispatcher->Enqueue<WindowShouldCloseEvent>();
		}
		return 0;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(hWnd, &ps);
		::EndPaint(hWnd, &ps);
		UNREFERENCED_PARAMETER(hdc);
		break;
	}
	case WM_KEYDOWN: {
		return 0;
	}
	case WM_CHAR: {
		return 0;
	}
	case WM_ENTERSIZEMOVE: {
		if (window) {
			window->eventDispatcher->Enqueue<ViewWillStartLiveResizeEvent>();
		}
		return 0;
	}
	case WM_EXITSIZEMOVE: {
		if (window) {
			window->eventDispatcher->Enqueue<ViewDidEndLiveResizeEvent>();
		}
		return 0;
	}
	case WM_SIZING: {
		if (window) {
			window->eventDispatcher->Enqueue<ViewNeedsUpdateSurfaceEvent>();
		}
		return TRUE;
	}
	case WM_SIZE: {
		if (window) {
			window->clientBounds.Width = static_cast<std::int16_t>(LOWORD(lParam));
			window->clientBounds.Height = static_cast<std::int16_t>(HIWORD(lParam));
		}
		break;
	}
	case WM_MOVE: {
		if (window) {
			window->clientBounds.X = static_cast<std::int16_t>(LOWORD(lParam));
			window->clientBounds.Y = static_cast<std::int16_t>(HIWORD(lParam));
		}
		break;
	}
	case WM_ACTIVATE: {
		break;
	}
	case WM_DESTROY: {
		const int exitCode = 0;
		::PostQuitMessage(exitCode);
		if (window) {
			window->eventDispatcher->Enqueue<WindowWillCloseEvent>();
		}
		return 0;
	}
	case WM_SETCURSOR: {
		auto hitTest= lParam & 0xffff;
		if (hitTest == HTCLIENT && window->gameCursor) {
			SetCursor(*window->gameCursor);
			return FALSE;
		}
		break;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
//-----------------------------------------------------------------------
GameWindowWin32::GameWindowWin32(HINSTANCE hInstance, int nCmdShow,
	HICON icon, HICON iconSmall, bool useOpenGL,
	std::shared_ptr<SystemEventDispatcher> const& eventDispatcher,
	PresentationParameters const& presentationParameters)
	: impl(std::make_unique<Impl>(hInstance, nCmdShow, icon, iconSmall, useOpenGL, eventDispatcher, presentationParameters))
{
}
//-----------------------------------------------------------------------
GameWindowWin32::~GameWindowWin32() = default;
//-----------------------------------------------------------------------
bool GameWindowWin32::AllowPlayerResizing() const
{
	POMDOG_ASSERT(impl);
	return impl->allowPlayerResizing;
}
//-----------------------------------------------------------------------
void GameWindowWin32::AllowPlayerResizing(bool allowResizing)
{
	POMDOG_ASSERT(impl);
	impl->SetAllowPlayerResizing(allowResizing);
}
//-----------------------------------------------------------------------
std::string GameWindowWin32::Title() const
{
	POMDOG_ASSERT(impl);
	return impl->title;
}
//-----------------------------------------------------------------------
void GameWindowWin32::Title(std::string const& title)
{
	POMDOG_ASSERT(impl);
	impl->SetTitle(title);
}
//-----------------------------------------------------------------------
Rectangle GameWindowWin32::ClientBounds() const
{
	POMDOG_ASSERT(impl);
	return impl->clientBounds;
}
//-----------------------------------------------------------------------
void GameWindowWin32::ClientBounds(Rectangle const& clientBounds)
{
	POMDOG_ASSERT(impl);
	impl->SetClientBounds(clientBounds);
}
//-----------------------------------------------------------------------
bool GameWindowWin32::IsMouseCursorVisible() const
{
	POMDOG_ASSERT(impl);
	return impl->isMouseCursorVisible;
}
//-----------------------------------------------------------------------
void GameWindowWin32::IsMouseCursorVisible(bool visible)
{
	POMDOG_ASSERT(impl);
	impl->SetMouseCursorVisible(visible);
}
//-----------------------------------------------------------------------
void GameWindowWin32::SetMouseCursor(MouseCursor cursor)
{
	POMDOG_ASSERT(impl);
	impl->SetMouseCursor(cursor);
}
//-----------------------------------------------------------------------
bool GameWindowWin32::IsMinimized() const
{
	POMDOG_ASSERT(impl);
	return (IsIconic(impl->windowHandle) == TRUE);
}
//-----------------------------------------------------------------------
void GameWindowWin32::Close()
{
	POMDOG_ASSERT(impl);
	CloseWindow(impl->windowHandle);
}
//-----------------------------------------------------------------------
HWND GameWindowWin32::NativeWindowHandle() const
{
	POMDOG_ASSERT(impl);
	return impl->windowHandle;
}
//-----------------------------------------------------------------------
}// namespace Win32
}// namespace Detail
}// namespace Pomdog
