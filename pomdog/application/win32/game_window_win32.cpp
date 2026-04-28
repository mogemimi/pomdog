// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/win32/game_window_win32.h"
#include "pomdog/application/backends/system_event_queue.h"
#include "pomdog/application/backends/system_events.h"
#include "pomdog/application/mouse_cursor.h"
#include "pomdog/application/win32/dark_mode.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/input/win32/keyboard_win32.h"
#include "pomdog/input/win32/mouse_win32.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/utfcpp_headers.h"

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

class GameWindowWin32Impl final : public GameWindowWin32 {
private:
    std::shared_ptr<SystemEventQueue> eventQueue_;
    std::string title_;
    Rect2D clientBounds_;
    std::optional<HCURSOR> gameCursor_;
    HINSTANCE instanceHandle_ = nullptr;
    HWND windowHandle_ = nullptr;
    bool allowUserResizing_ = false;
    bool isFullScreen_ = false;
    bool isMouseCursorVisible_ = true;

public:
    ~GameWindowWin32Impl()
    {
        if (windowHandle_ != nullptr) {
            ::SetWindowLongPtr(windowHandle_, GWLP_USERDATA, 0);
            ::DestroyWindow(windowHandle_);
            windowHandle_ = nullptr;

            ::CoUninitialize();
        }

        instanceHandle_ = nullptr;
    }

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        HINSTANCE hInstance,
        int nCmdShow,
        HICON icon,
        HICON iconSmall,
        bool useOpenGL,
        const std::shared_ptr<SystemEventQueue>& eventQueueIn,
        const gpu::PresentationParameters& presentationParameters) noexcept
    {
        eventQueue_ = eventQueueIn;
        title_ = "Game";
        clientBounds_ = Rect2D{0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
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
            GameWindowWin32Impl::windowProcedure,
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

    bool
    getAllowUserResizing() const override
    {
        return allowUserResizing_;
    }

    void
    setAllowUserResizing(bool allowResizingIn) override
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
            return;
        }

        if (::SetWindowPos(
                windowHandle_, 0, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_FRAMECHANGED) == 0) {
            return;
        }

        allowUserResizing_ = allowResizingIn;
    }

    std::string
    getTitle() const override
    {
        return title_;
    }

    void
    setTitle(const std::string& titleIn) override
    {
        POMDOG_ASSERT(windowHandle_ != nullptr);

        // NOTE: Convert UTF-8 to UTF-16 (WCHAR) for the Windows Unicode API.
        // SetWindowTextW expects LPCWSTR; passing a UTF-8 const char* directly would
        // corrupt multi-byte characters such as CJK text.
        const int wideLen = ::MultiByteToWideChar(CP_UTF8, 0, titleIn.c_str(), -1, nullptr, 0);
        if (wideLen <= 0) {
            return;
        }
        std::wstring wideTitle(static_cast<std::size_t>(wideLen - 1), L'\0');
        if (::MultiByteToWideChar(CP_UTF8, 0, titleIn.c_str(), -1, wideTitle.data(), wideLen) == 0) {
            return;
        }

        if (::SetWindowTextW(windowHandle_, wideTitle.c_str()) == 0) {
            return;
        }

        title_ = titleIn;
    }

    Rect2D
    getClientBounds() const override
    {
        return clientBounds_;
    }

    void
    setClientBounds(const Rect2D& clientBoundsIn) override
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
            return;
        }

        clientBounds_.width = clientBoundsIn.width;
        clientBounds_.height = clientBoundsIn.height;

        eventQueue_->enqueue(SystemEvent{
            .kind = SystemEventKind::ViewWillStartLiveResizeEvent,
            .data = {},
        });
        eventQueue_->enqueue(SystemEvent{
            .kind = SystemEventKind::ViewDidEndLiveResizeEvent,
            .data = {},
        });
    }

    bool
    isMouseCursorVisible() const override
    {
        return isMouseCursorVisible_;
    }

    void
    setMouseCursorVisible(bool visibleIn) override
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

    void
    setMouseCursor(MouseCursor cursorIn) override
    {
        gameCursor_ = ::LoadCursor(nullptr, toStandardCursorID(cursorIn));

        if (isMouseCursorVisible_) {
            ::SetCursor(*gameCursor_);
        }
    }

    bool
    isMinimized() const override
    {
        return (::IsIconic(windowHandle_) == TRUE);
    }

    void
    close() override
    {
        ::CloseWindow(windowHandle_);
    }

    HWND
    getNativeWindowHandle() const override
    {
        return windowHandle_;
    }

private:
    static LRESULT CALLBACK
    windowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        auto window = reinterpret_cast<GameWindowWin32Impl*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

        POMDOG_ASSERT(WM_QUIT != msg);

        switch (msg) {
        case WM_CLOSE: {
            if (window) {
                window->eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::WindowShouldCloseEvent,
                    .data = {},
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
                if (msg == WM_UNICHAR) {
                    // NOTE: WM_UNICHAR provides a UTF-32 code point.
                    const auto codePoint = static_cast<char32_t>(wParam);
                    utf8::append(codePoint, std::back_inserter(text));
                }
                else {
                    // NOTE: WM_CHAR/WM_SYSCHAR provides a UTF-16 code unit.
                    const auto ch = static_cast<wchar_t>(wParam);
                    std::wstring wstr(1, ch);
                    utf8::utf16to8(wstr.begin(), wstr.end(), std::back_inserter(text));
                }
                if (!text.empty()) {
                    window->eventQueue_->enqueue(SystemEvent{
                        .kind = SystemEventKind::InputTextEvent,
                        .data = InputTextEvent{
                            .text = std::move(text),
                        },
                    });
                }
            }
            return 0;
        }
        case WM_ENTERSIZEMOVE: {
            if (window) {
                window->eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::ViewWillStartLiveResizeEvent,
                    .data = {},
                });
            }
            return 0;
        }
        case WM_EXITSIZEMOVE: {
            if (window) {
                window->eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::ViewDidEndLiveResizeEvent,
                    .data = {},
                });
            }
            return 0;
        }
        case WM_SIZING: {
            if (window) {
                window->eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::ViewNeedsUpdateSurfaceEvent,
                    .data = {},
                });
            }
            return TRUE;
        }
        case WM_SIZE: {
            if (window) {
                window->clientBounds_.width = static_cast<i32>(LOWORD(lParam));
                window->clientBounds_.height = static_cast<i32>(HIWORD(lParam));
            }
            break;
        }
        case WM_MOVE: {
            if (window) {
                window->clientBounds_.x = static_cast<i32>(LOWORD(lParam));
                window->clientBounds_.y = static_cast<i32>(HIWORD(lParam));
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
                    .data = {},
                });
            }
            return 0;
        }
        case WM_SETCURSOR: {
            if (window) {
                const auto hitTest = lParam & 0xffff;
                if (hitTest == HTCLIENT) {
                    if (!window->isMouseCursorVisible_) {
                        ::SetCursor(nullptr);
                        return FALSE;
                    }
                    if (window->gameCursor_) {
                        ::SetCursor(*window->gameCursor_);
                        return FALSE;
                    }
                }
            }
            break;
        }
        case WM_INPUT: {
            if (window) {
                RAWINPUT raw = {};
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
        case WM_MOUSEMOVE: {
            if (window) {
                TRACKMOUSEEVENT tme = {};
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hWnd;
                ::TrackMouseEvent(&tme);

                window->eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::MouseEnteredEvent,
                    .data = MousePositionEvent{},
                });
            }
            break;
        }
        case WM_MOUSELEAVE: {
            if (window) {
                window->eventQueue_->enqueue(SystemEvent{
                    .kind = SystemEventKind::MouseExitedEvent,
                    .data = MousePositionEvent{},
                });
            }
            break;
        }
        default:
            break;
        }

        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
};

} // namespace

GameWindowWin32::GameWindowWin32() = default;

GameWindowWin32::~GameWindowWin32() = default;

std::tuple<std::shared_ptr<GameWindowWin32>, std::unique_ptr<Error>>
GameWindowWin32::create(
    HINSTANCE hInstance,
    int nCmdShow,
    HICON icon,
    HICON iconSmall,
    bool useOpenGL,
    const std::shared_ptr<SystemEventQueue>& eventQueue,
    const gpu::PresentationParameters& presentationParameters) noexcept
{
    auto window = std::make_shared<GameWindowWin32Impl>();
    if (auto err = window->initialize(
            hInstance,
            nCmdShow,
            icon,
            iconSmall,
            useOpenGL,
            eventQueue,
            presentationParameters);
        err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(window), nullptr);
}

} // namespace pomdog::detail::win32
