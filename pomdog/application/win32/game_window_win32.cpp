// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/win32/game_window_win32.h"
#include "pomdog/application/mouse_cursor.h"
#include "pomdog/application/system_events.h"
#include "pomdog/application/win32/dark_mode.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/input/win32/keyboard_win32.h"
#include "pomdog/input/win32/mouse_win32.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <objbase.h>
#include <array>
#include <optional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::win32 {
namespace {

template <typename T>
LPTSTR MakeIntegerResource(T&& resource) noexcept
{
#pragma warning(push)
#pragma warning(disable : 4302)
    return MAKEINTRESOURCE(std::forward<T>(resource));
#pragma warning(pop)
}

LPCTSTR ToStandardCursorID(MouseCursor cursor) noexcept
{
    switch (cursor) {
    case MouseCursor::Arrow:
        return IDC_ARROW;
    case MouseCursor::IBeam:
        return IDC_IBEAM;
    case MouseCursor::PointingHand:
        return IDC_HAND;
    case MouseCursor::ResizeHorizontal:
        return IDC_SIZEWE;
    case MouseCursor::ResizeVertical:
        return IDC_SIZENS;
    }
    return IDC_ARROW;
}

[[nodiscard]] std::unique_ptr<Error>
RegisterInputDevices(HWND windowHandle) noexcept
{
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT)0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT)0x02)
#endif
#ifndef HID_USAGE_GENERIC_KEYBOARD
#define HID_USAGE_GENERIC_KEYBOARD ((USHORT)0x06)
#endif

    std::array<RAWINPUTDEVICE, 2> inputDevices;

    auto& mouse = inputDevices[0];
    mouse.usUsagePage = HID_USAGE_PAGE_GENERIC;
    mouse.usUsage = HID_USAGE_GENERIC_MOUSE;
    mouse.dwFlags = 0;
    mouse.hwndTarget = windowHandle;

    auto& keyboard = inputDevices[1];
    keyboard.usUsagePage = HID_USAGE_PAGE_GENERIC;
    keyboard.usUsage = HID_USAGE_GENERIC_KEYBOARD;
    keyboard.dwFlags = 0;
    keyboard.hwndTarget = windowHandle;

    BOOL success = ::RegisterRawInputDevices(
        inputDevices.data(),
        static_cast<UINT>(inputDevices.size()),
        static_cast<UINT>(sizeof(inputDevices[0])));

    if (success == FALSE) {
        return errors::New("RegisterRawInputDevices() failed");
    }

    return nullptr;
}

} // namespace

class GameWindowWin32::Impl final {
public:
    ~Impl();

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        HINSTANCE hInstance,
        int nCmdShow,
        HICON icon,
        HICON iconSmall,
        bool useOpenGL,
        const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
        const gpu::PresentationParameters& presentationParameters) noexcept;

    void SetAllowUserResizing(bool allowResizing);

    void SetTitle(const std::string& caption);

    void SetClientBounds(const Rectangle& clientBounds);

    void SetMouseCursorVisible(bool visible);

    void SetMouseCursor(MouseCursor cursor);

private:
    static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
    std::shared_ptr<EventQueue<SystemEvent>> eventQueue;
    std::string title;
    Rectangle clientBounds;
    std::optional<HCURSOR> gameCursor;
    HINSTANCE instanceHandle = nullptr;
    HWND windowHandle = nullptr;
    bool allowUserResizing = false;
    bool isFullScreen = false;
    bool isMouseCursorVisible = true;
};

std::unique_ptr<Error>
GameWindowWin32::Impl::Initialize(
    HINSTANCE hInstance,
    int nCmdShow,
    HICON icon,
    HICON iconSmall,
    bool useOpenGL,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueueIn,
    const gpu::PresentationParameters& presentationParameters) noexcept
{
    eventQueue = eventQueueIn;
    title = "Game";
    clientBounds = Rectangle{0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    instanceHandle = hInstance;
    windowHandle = nullptr;
    allowUserResizing = false;
    isFullScreen = presentationParameters.IsFullScreen;
    isMouseCursorVisible = true;

    POMDOG_ASSERT(clientBounds.width > 0);
    POMDOG_ASSERT(clientBounds.height > 0);

    DWORD windowStyle = 0;
    DWORD windowStyleEx = 0;
    LONG adjustedWidth = static_cast<LONG>(clientBounds.width);
    LONG adjustedHeight = static_cast<LONG>(clientBounds.height);

    if (useOpenGL) {
        windowStyle |= WS_CLIPCHILDREN;
        windowStyle |= WS_CLIPSIBLINGS;
    }

    if (isFullScreen) {
        windowStyleEx |= WS_EX_TOPMOST;
        windowStyle |= WS_POPUP;
        clientBounds.X = 0;
        clientBounds.Y = 0;
    }
    else {
        DWORD const fixedWindowStyle(WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
        windowStyle |= fixedWindowStyle;

        RECT windowRect = {0, 0, static_cast<LONG>(clientBounds.width), static_cast<LONG>(clientBounds.height)};
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
        0,
        0,
        instanceHandle,
        icon,
        LoadCursor(nullptr, IDC_ARROW),
        static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)),
        nullptr,
        windowName,
        iconSmall,
    };

    if (::RegisterClassEx(&wcex) == 0) {
        return errors::New("RegisterClassEx() failed");
    }

    windowHandle = CreateWindowEx(
        windowStyleEx,
        wcex.lpszClassName,
        title.data(),
        windowStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        adjustedWidth,
        adjustedHeight,
        nullptr,
        nullptr,
        instanceHandle,
        nullptr);

    if (windowHandle == nullptr) {
        return errors::New("CreateWindowEx() failed");
    }

    if (IsDarkMode()) {
        if (auto err = UseImmersiveDarkMode(windowHandle, true); err != nullptr) {
            return errors::Wrap(std::move(err), "UseImmersiveDarkMode() failed");
        }
    }

    // NOTE: See http://msdn.microsoft.com/ja-jp/library/ff485844(v=vs.85).aspx
    if (auto hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE); FAILED(hr)) {
        return errors::New("CoInitializeEx() failed");
    }

    if (nCmdShow == SW_MAXIMIZE) {
        ::ShowWindow(windowHandle, SW_RESTORE);
    }
    else {
        ::ShowWindow(windowHandle, nCmdShow);
    }

    {
        POINT point = {0, 0};
        if (0 != ::ClientToScreen(windowHandle, &point)) {
            clientBounds.X = static_cast<std::int32_t>(point.x);
            clientBounds.Y = static_cast<std::int32_t>(point.y);
        }
    }

    ::SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    if (auto err = RegisterInputDevices(windowHandle); err != nullptr) {
        return errors::Wrap(std::move(err), "RegisterInputDevices() failed");
    }

    return nullptr;
}

GameWindowWin32::Impl::~Impl()
{
    if (windowHandle != nullptr) {
        ::DestroyWindow(windowHandle);
        ::SetWindowLongPtr(windowHandle, GWLP_USERDATA, 0);
        windowHandle = nullptr;

        ::CoUninitialize();
    }

    instanceHandle = nullptr;
}

void GameWindowWin32::Impl::SetAllowUserResizing(bool allowResizingIn)
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

    if (0 == ::SetWindowLongPtr(windowHandle, GWL_STYLE, windowStyle)) {
        ///@todo Not implemented
        return;
    }

    if (::SetWindowPos(
            windowHandle, 0, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_FRAMECHANGED) == 0) {
        ///@todo Not implemented
        return;
    }

    allowUserResizing = allowResizingIn;
}

void GameWindowWin32::Impl::SetTitle(const std::string& titleIn)
{
    POMDOG_ASSERT(nullptr != windowHandle);

    if (0 == ::SetWindowText(windowHandle, titleIn.c_str())) {
        ///@todo Not implemented
        return;
    }

    this->title = titleIn;
}

void GameWindowWin32::Impl::SetClientBounds(const Rectangle& clientBoundsIn)
{
    POMDOG_ASSERT(nullptr != windowHandle);

    if (isFullScreen) {
        return;
    }

    DWORD const dwStyle = static_cast<DWORD>(::GetWindowLong(windowHandle, GWL_STYLE));

    RECT windowRect = {0, 0, clientBoundsIn.width, clientBoundsIn.height};

    AdjustWindowRect(&windowRect, dwStyle, FALSE);

    int const adjustedWidth = static_cast<int>(windowRect.right - windowRect.left);
    int const adjustedHeight = static_cast<int>(windowRect.bottom - windowRect.top);
    UINT const flags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE;

    if (0 == ::SetWindowPos(windowHandle, 0, 0, 0, adjustedWidth, adjustedHeight, flags)) {
        ///@todo Not implemented
        return;
    }

    clientBounds.width = clientBoundsIn.width;
    clientBounds.height = clientBoundsIn.height;

    eventQueue->Enqueue(SystemEvent{
        .Kind = SystemEventKind::ViewWillStartLiveResizeEvent,
    });
    eventQueue->Enqueue(SystemEvent{
        .Kind = SystemEventKind::ViewDidEndLiveResizeEvent,
    });
}

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

void GameWindowWin32::Impl::SetMouseCursor(MouseCursor cursorIn)
{
    gameCursor = LoadCursor(nullptr, ToStandardCursorID(cursorIn));

    if (isMouseCursorVisible) {
        ::SetCursor(*gameCursor);
    }
}

LRESULT CALLBACK
GameWindowWin32::Impl::WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto window = reinterpret_cast<GameWindowWin32::Impl*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

    POMDOG_ASSERT(WM_QUIT != msg);

    switch (msg) {
    case WM_CLOSE: {
        if (window) {
            window->eventQueue->Enqueue(SystemEvent{
                .Kind = SystemEventKind::WindowShouldCloseEvent,
            });
        }
        return 0;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        [[maybe_unused]] HDC hdc = ::BeginPaint(hWnd, &ps);
        ::EndPaint(hWnd, &ps);
        break;
    }
    case WM_KEYDOWN: {
        return 0;
    }
    case WM_UNICHAR:
        [[fallthrough]];
    case WM_SYSCHAR:
        [[fallthrough]];
    case WM_CHAR: {
        if ((msg == WM_UNICHAR) && (wParam == UNICODE_NOCHAR)) {
            // TODO: Not implemented
            return TRUE;
        }

        if (window) {
            std::string text;
            text += static_cast<char>(wParam);
            window->eventQueue->Enqueue(SystemEvent{
                .Kind = SystemEventKind::InputTextEvent,
                .Data = InputTextEvent{
                    .Text = std::move(text),
                },
            });
        }
        return 0;
    }
    case WM_ENTERSIZEMOVE: {
        if (window) {
            window->eventQueue->Enqueue(SystemEvent{
                .Kind = SystemEventKind::ViewWillStartLiveResizeEvent,
            });
        }
        return 0;
    }
    case WM_EXITSIZEMOVE: {
        if (window) {
            window->eventQueue->Enqueue(SystemEvent{
                .Kind = SystemEventKind::ViewDidEndLiveResizeEvent,
            });
        }
        return 0;
    }
    case WM_SIZING: {
        if (window) {
            window->eventQueue->Enqueue(SystemEvent{
                .Kind = SystemEventKind::ViewNeedsUpdateSurfaceEvent,
            });
        }
        return TRUE;
    }
    case WM_SIZE: {
        if (window) {
            window->clientBounds.width = static_cast<std::int16_t>(LOWORD(lParam));
            window->clientBounds.height = static_cast<std::int16_t>(HIWORD(lParam));
        }
        break;
    }
    case WM_MOVE: {
        if (window) {
            window->clientBounds.x = static_cast<std::int16_t>(LOWORD(lParam));
            window->clientBounds.y = static_cast<std::int16_t>(HIWORD(lParam));
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
            window->eventQueue->Enqueue(SystemEvent{
                .Kind = SystemEventKind::WindowWillCloseEvent,
            });
        }
        return 0;
    }
    case WM_SETCURSOR: {
        if (window) {
            const auto hitTest = lParam & 0xffff;
            if (hitTest == HTCLIENT && window->gameCursor) {
                SetCursor(*window->gameCursor);
                return FALSE;
            }
        }
        break;
    }
    case WM_INPUT: {
        if (window) {
            static RAWINPUT raw;
            UINT size = sizeof(raw);

            GetRawInputData((HRAWINPUT)lParam, RID_INPUT,
                &raw, &size, sizeof(RAWINPUTHEADER));

            if (raw.header.dwType == RIM_TYPEMOUSE) {
                TranslateMouseEvent(window->windowHandle, raw.data.mouse, window->eventQueue);
            }
            else if (raw.header.dwType == RIM_TYPEKEYBOARD) {
                TranslateKeyboardEvent(raw.data.keyboard, window->eventQueue);
            }
        }
        break;
    }
    default:
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

GameWindowWin32::GameWindowWin32()
    : impl(std::make_unique<Impl>())
{
}

GameWindowWin32::~GameWindowWin32() = default;

std::unique_ptr<Error>
GameWindowWin32::Initialize(
    HINSTANCE hInstance,
    int nCmdShow,
    HICON icon,
    HICON iconSmall,
    bool useOpenGL,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
    const gpu::PresentationParameters& presentationParameters) noexcept
{
    POMDOG_ASSERT(impl != nullptr);
    return impl->Initialize(
        hInstance,
        nCmdShow,
        icon,
        iconSmall,
        useOpenGL,
        eventQueue,
        presentationParameters);
}

bool GameWindowWin32::GetAllowUserResizing() const
{
    POMDOG_ASSERT(impl);
    return impl->allowUserResizing;
}

void GameWindowWin32::SetAllowUserResizing(bool allowResizing)
{
    POMDOG_ASSERT(impl);
    impl->SetAllowUserResizing(allowResizing);
}

std::string GameWindowWin32::GetTitle() const
{
    POMDOG_ASSERT(impl);
    return impl->title;
}

void GameWindowWin32::SetTitle(const std::string& title)
{
    POMDOG_ASSERT(impl);
    impl->SetTitle(title);
}

Rectangle GameWindowWin32::GetClientBounds() const
{
    POMDOG_ASSERT(impl);
    return impl->clientBounds;
}

void GameWindowWin32::SetClientBounds(const Rectangle& clientBounds)
{
    POMDOG_ASSERT(impl);
    impl->SetClientBounds(clientBounds);
}

bool GameWindowWin32::IsMouseCursorVisible() const
{
    POMDOG_ASSERT(impl);
    return impl->isMouseCursorVisible;
}

void GameWindowWin32::SetMouseCursorVisible(bool visible)
{
    POMDOG_ASSERT(impl);
    impl->SetMouseCursorVisible(visible);
}

void GameWindowWin32::SetMouseCursor(MouseCursor cursor)
{
    POMDOG_ASSERT(impl);
    impl->SetMouseCursor(cursor);
}

bool GameWindowWin32::IsMinimized() const
{
    POMDOG_ASSERT(impl);
    return (IsIconic(impl->windowHandle) == TRUE);
}

void GameWindowWin32::Close()
{
    POMDOG_ASSERT(impl);
    CloseWindow(impl->windowHandle);
}

HWND GameWindowWin32::GetNativeWindowHandle() const
{
    POMDOG_ASSERT(impl);
    return impl->windowHandle;
}

} // namespace pomdog::detail::win32
