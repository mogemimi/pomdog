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
[[nodiscard]] LPTSTR
makeIntegerResource(T&& resource) noexcept
{
#pragma warning(push)
#pragma warning(disable : 4302)
    return MAKEINTRESOURCE(std::forward<T>(resource));
#pragma warning(pop)
}

[[nodiscard]] LPCTSTR
toStandardCursorID(MouseCursor cursor) noexcept
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
registerInputDevices(HWND windowHandle) noexcept
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
        return errors::make("RegisterRawInputDevices() failed");
    }

    return nullptr;
}

} // namespace

class GameWindowWin32::Impl final {
public:
    ~Impl();

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        HINSTANCE hInstance,
        int nCmdShow,
        HICON icon,
        HICON iconSmall,
        bool useOpenGL,
        const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
        const gpu::PresentationParameters& presentationParameters) noexcept;

    void setAllowUserResizing(bool allowResizing);

    void setTitle(const std::string& caption);

    void setClientBounds(const Rectangle& clientBounds);

    void setMouseCursorVisible(bool visible);

    void setMouseCursor(MouseCursor cursor);

private:
    static LRESULT CALLBACK
    windowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
    std::shared_ptr<EventQueue<SystemEvent>> eventQueue_;
    std::string title_;
    Rectangle clientBounds_;
    std::optional<HCURSOR> gameCursor_;
    HINSTANCE instanceHandle_ = nullptr;
    HWND windowHandle_ = nullptr;
    bool allowUserResizing_ = false;
    bool isFullScreen_ = false;
    bool isMouseCursorVisible_ = true;
};

std::unique_ptr<Error>
GameWindowWin32::Impl::initialize(
    HINSTANCE hInstance,
    int nCmdShow,
    HICON icon,
    HICON iconSmall,
    bool useOpenGL,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueueIn,
    const gpu::PresentationParameters& presentationParameters) noexcept
{
    eventQueue_ = eventQueueIn;
    title_ = "Game";
    clientBounds_ = Rectangle{0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    instanceHandle_ = hInstance;
    windowHandle_ = nullptr;
    allowUserResizing_ = false;
    isFullScreen_ = presentationParameters.isFullScreen;
    isMouseCursorVisible_ = true;

    POMDOG_ASSERT(clientBounds_.width > 0);
    POMDOG_ASSERT(clientBounds_.height > 0);

    DWORD windowStyle = 0;
    DWORD windowStyleEx = 0;
    LONG adjustedWidth = static_cast<LONG>(clientBounds_.width);
    LONG adjustedHeight = static_cast<LONG>(clientBounds_.height);

    if (useOpenGL) {
        windowStyle |= WS_CLIPCHILDREN;
        windowStyle |= WS_CLIPSIBLINGS;
    }

    if (isFullScreen_) {
        windowStyleEx |= WS_EX_TOPMOST;
        windowStyle |= WS_POPUP;
        clientBounds_.x = 0;
        clientBounds_.y = 0;
    }
    else {
        constexpr DWORD fixedWindowStyle = (WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
        windowStyle |= fixedWindowStyle;

        RECT windowRect = {0, 0, static_cast<LONG>(clientBounds_.width), static_cast<LONG>(clientBounds_.height)};
        ::AdjustWindowRect(&windowRect, windowStyle, FALSE);

        adjustedWidth = windowRect.right - windowRect.left;
        adjustedHeight = windowRect.bottom - windowRect.top;
    }

    constexpr auto windowName = "Pomdog.GameWindowWin32";

    UINT windowClassStyle = (CS_HREDRAW | CS_VREDRAW);

    if (useOpenGL) {
        windowClassStyle |= CS_OWNDC;
    }

    if (icon == nullptr) {
        icon = ::LoadIcon(instanceHandle_, makeIntegerResource(IDI_APPLICATION));
    }

    if (iconSmall == nullptr) {
        iconSmall = ::LoadIcon(instanceHandle_, makeIntegerResource(IDI_APPLICATION));
    }

    WNDCLASSEX wcex = {
        sizeof(WNDCLASSEX),
        windowClassStyle,
        Impl::windowProcedure,
        0,
        0,
        instanceHandle_,
        icon,
        ::LoadCursor(nullptr, IDC_ARROW),
        static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH)),
        nullptr,
        windowName,
        iconSmall,
    };

    if (::RegisterClassEx(&wcex) == 0) {
        return errors::make("RegisterClassEx() failed");
    }

    windowHandle_ = CreateWindowEx(
        windowStyleEx,
        wcex.lpszClassName,
        title_.data(),
        windowStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        adjustedWidth,
        adjustedHeight,
        nullptr,
        nullptr,
        instanceHandle_,
        nullptr);

    if (windowHandle_ == nullptr) {
        return errors::make("CreateWindowEx() failed");
    }

    if (isDarkMode()) {
        if (auto err = useImmersiveDarkMode(windowHandle_, true); err != nullptr) {
            return errors::wrap(std::move(err), "useImmersiveDarkMode() failed");
        }
    }

    // NOTE: See http://msdn.microsoft.com/ja-jp/library/ff485844(v=vs.85).aspx
    if (auto hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE); FAILED(hr)) {
        return errors::make("CoInitializeEx() failed");
    }

    if (nCmdShow == SW_MAXIMIZE) {
        ::ShowWindow(windowHandle_, SW_RESTORE);
    }
    else {
        ::ShowWindow(windowHandle_, nCmdShow);
    }

    {
        POINT point = {0, 0};
        if (0 != ::ClientToScreen(windowHandle_, &point)) {
            clientBounds_.x = static_cast<std::int32_t>(point.x);
            clientBounds_.y = static_cast<std::int32_t>(point.y);
        }
    }

    ::SetWindowLongPtr(windowHandle_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    if (auto err = registerInputDevices(windowHandle_); err != nullptr) {
        return errors::wrap(std::move(err), "registerInputDevices() failed");
    }

    return nullptr;
}

GameWindowWin32::Impl::~Impl()
{
    if (windowHandle_ != nullptr) {
        ::DestroyWindow(windowHandle_);
        ::SetWindowLongPtr(windowHandle_, GWLP_USERDATA, 0);
        windowHandle_ = nullptr;

        ::CoUninitialize();
    }

    instanceHandle_ = nullptr;
}

void GameWindowWin32::Impl::setAllowUserResizing(bool allowResizingIn)
{
    POMDOG_ASSERT(windowHandle_ != nullptr);

    if (isFullScreen_) {
        return;
    }

    LONG_PTR windowStyle = ::GetWindowLongPtr(windowHandle_, GWL_STYLE);
    if (0 == windowStyle) {
        return;
    }

    if (allowResizingIn) {
        windowStyle |= WS_THICKFRAME;
    }
    else {
        windowStyle &= ~WS_THICKFRAME;
    }

    if (0 == ::SetWindowLongPtr(windowHandle_, GWL_STYLE, windowStyle)) {
        ///@todo Not implemented
        return;
    }

    if (::SetWindowPos(
            windowHandle_, 0, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_FRAMECHANGED) == 0) {
        ///@todo Not implemented
        return;
    }

    allowUserResizing_ = allowResizingIn;
}

void GameWindowWin32::Impl::setTitle(const std::string& titleIn)
{
    POMDOG_ASSERT(windowHandle_ != nullptr);

    if (0 == ::SetWindowText(windowHandle_, titleIn.c_str())) {
        ///@todo Not implemented
        return;
    }

    title_ = titleIn;
}

void GameWindowWin32::Impl::setClientBounds(const Rectangle& clientBoundsIn)
{
    POMDOG_ASSERT(windowHandle_ != nullptr);

    if (isFullScreen_) {
        return;
    }

    DWORD const dwStyle = static_cast<DWORD>(::GetWindowLong(windowHandle_, GWL_STYLE));

    RECT windowRect = {0, 0, clientBoundsIn.width, clientBoundsIn.height};

    AdjustWindowRect(&windowRect, dwStyle, FALSE);

    int const adjustedWidth = static_cast<int>(windowRect.right - windowRect.left);
    int const adjustedHeight = static_cast<int>(windowRect.bottom - windowRect.top);
    constexpr UINT flags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE;

    if (0 == ::SetWindowPos(windowHandle_, 0, 0, 0, adjustedWidth, adjustedHeight, flags)) {
        ///@todo Not implemented
        return;
    }

    clientBounds_.width = clientBoundsIn.width;
    clientBounds_.height = clientBoundsIn.height;

    eventQueue_->enqueue(SystemEvent{
        .kind = SystemEventKind::ViewWillStartLiveResizeEvent,
    });
    eventQueue_->enqueue(SystemEvent{
        .kind = SystemEventKind::ViewDidEndLiveResizeEvent,
    });
}

void GameWindowWin32::Impl::setMouseCursorVisible(bool visibleIn)
{
    isMouseCursorVisible_ = visibleIn;

    if (isMouseCursorVisible_) {
        if (gameCursor_) {
            ::SetCursor(*gameCursor_);
        }
        else {
            auto nativeCursor = ::LoadCursor(nullptr, toStandardCursorID(MouseCursor::Arrow));
            ::SetCursor(nativeCursor);
        }
    }
    else {
        ::SetCursor(nullptr);
    }
}

void GameWindowWin32::Impl::setMouseCursor(MouseCursor cursorIn)
{
    gameCursor_ = ::LoadCursor(nullptr, toStandardCursorID(cursorIn));

    if (isMouseCursorVisible_) {
        ::SetCursor(*gameCursor_);
    }
}

LRESULT CALLBACK
GameWindowWin32::Impl::windowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto window = reinterpret_cast<GameWindowWin32::Impl*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

    POMDOG_ASSERT(WM_QUIT != msg);

    switch (msg) {
    case WM_CLOSE: {
        if (window) {
            window->eventQueue_->enqueue(SystemEvent{
                .kind = SystemEventKind::WindowShouldCloseEvent,
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
            window->eventQueue_->enqueue(SystemEvent{
                .kind = SystemEventKind::InputTextEvent,
                .data = InputTextEvent{
                    .text = std::move(text),
                },
            });
        }
        return 0;
    }
    case WM_ENTERSIZEMOVE: {
        if (window) {
            window->eventQueue_->enqueue(SystemEvent{
                .kind = SystemEventKind::ViewWillStartLiveResizeEvent,
            });
        }
        return 0;
    }
    case WM_EXITSIZEMOVE: {
        if (window) {
            window->eventQueue_->enqueue(SystemEvent{
                .kind = SystemEventKind::ViewDidEndLiveResizeEvent,
            });
        }
        return 0;
    }
    case WM_SIZING: {
        if (window) {
            window->eventQueue_->enqueue(SystemEvent{
                .kind = SystemEventKind::ViewNeedsUpdateSurfaceEvent,
            });
        }
        return TRUE;
    }
    case WM_SIZE: {
        if (window) {
            window->clientBounds_.width = static_cast<std::int16_t>(LOWORD(lParam));
            window->clientBounds_.height = static_cast<std::int16_t>(HIWORD(lParam));
        }
        break;
    }
    case WM_MOVE: {
        if (window) {
            window->clientBounds_.x = static_cast<std::int16_t>(LOWORD(lParam));
            window->clientBounds_.y = static_cast<std::int16_t>(HIWORD(lParam));
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
            window->eventQueue_->enqueue(SystemEvent{
                .kind = SystemEventKind::WindowWillCloseEvent,
            });
        }
        return 0;
    }
    case WM_SETCURSOR: {
        if (window) {
            const auto hitTest = lParam & 0xffff;
            if (hitTest == HTCLIENT && window->gameCursor_) {
                ::SetCursor(*window->gameCursor_);
                return FALSE;
            }
        }
        break;
    }
    case WM_INPUT: {
        if (window) {
            static RAWINPUT raw;
            UINT size = sizeof(raw);

            ::GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));

            if (raw.header.dwType == RIM_TYPEMOUSE) {
                translateMouseEvent(window->windowHandle_, raw.data.mouse, window->eventQueue_);
            }
            else if (raw.header.dwType == RIM_TYPEKEYBOARD) {
                translateKeyboardEvent(raw.data.keyboard, window->eventQueue_);
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
    : impl_(std::make_unique<Impl>())
{
}

GameWindowWin32::~GameWindowWin32() = default;

std::unique_ptr<Error>
GameWindowWin32::initialize(
    HINSTANCE hInstance,
    int nCmdShow,
    HICON icon,
    HICON iconSmall,
    bool useOpenGL,
    const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue,
    const gpu::PresentationParameters& presentationParameters) noexcept
{
    POMDOG_ASSERT(impl_ != nullptr);
    return impl_->initialize(
        hInstance,
        nCmdShow,
        icon,
        iconSmall,
        useOpenGL,
        eventQueue,
        presentationParameters);
}

bool GameWindowWin32::getAllowUserResizing() const
{
    POMDOG_ASSERT(impl_);
    return impl_->allowUserResizing_;
}

void GameWindowWin32::setAllowUserResizing(bool allowResizing)
{
    POMDOG_ASSERT(impl_);
    impl_->setAllowUserResizing(allowResizing);
}

std::string GameWindowWin32::getTitle() const
{
    POMDOG_ASSERT(impl_);
    return impl_->title_;
}

void GameWindowWin32::setTitle(const std::string& title)
{
    POMDOG_ASSERT(impl_);
    impl_->setTitle(title);
}

Rectangle GameWindowWin32::getClientBounds() const
{
    POMDOG_ASSERT(impl_);
    return impl_->clientBounds_;
}

void GameWindowWin32::setClientBounds(const Rectangle& clientBounds)
{
    POMDOG_ASSERT(impl_);
    impl_->setClientBounds(clientBounds);
}

bool GameWindowWin32::isMouseCursorVisible() const
{
    POMDOG_ASSERT(impl_);
    return impl_->isMouseCursorVisible_;
}

void GameWindowWin32::setMouseCursorVisible(bool visible)
{
    POMDOG_ASSERT(impl_);
    impl_->setMouseCursorVisible(visible);
}

void GameWindowWin32::setMouseCursor(MouseCursor cursor)
{
    POMDOG_ASSERT(impl_);
    impl_->setMouseCursor(cursor);
}

bool GameWindowWin32::isMinimized() const
{
    POMDOG_ASSERT(impl_);
    return (::IsIconic(impl_->windowHandle_) == TRUE);
}

void GameWindowWin32::close()
{
    POMDOG_ASSERT(impl_);
    ::CloseWindow(impl_->windowHandle_);
}

HWND GameWindowWin32::getNativeWindowHandle() const
{
    POMDOG_ASSERT(impl_);
    return impl_->windowHandle_;
}

} // namespace pomdog::detail::win32
