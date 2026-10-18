// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/basic/win32/windows_headers.h"
#include "pomdog/experimental/clipboard/clipboard_service.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class GameWindow;
} // namespace pomdog

namespace pomdog::clipboard::detail::win32 {

/// Implements `ClipboardService` with the Win32 clipboard and CF_UNICODETEXT.
///
/// The game window owns what the service writes. Requests finish at once, and
/// their events arrive after the next `update()`.
class ClipboardServiceWin32 final : public ClipboardService {
public:
    /// Creates a service whose clipboard owner is windowHandle, the native
    /// window of window.
    ClipboardServiceWin32(std::shared_ptr<GameWindow> window, HWND windowHandle) noexcept;

    ClipboardServiceWin32(const ClipboardServiceWin32&) = delete;
    ClipboardServiceWin32& operator=(const ClipboardServiceWin32&) = delete;

    ~ClipboardServiceWin32() override;

    [[nodiscard]] ClipboardCapabilities
    getCapabilities() const noexcept override;

    void
    update() override;

    [[nodiscard]] std::span<const ClipboardEvent>
    getEvents() const noexcept override;

    u64
    requestWriteText(std::string_view text) override;

    u64
    requestReadText() override;

private:
    [[nodiscard]] ClipboardStatus
    writeText(std::string_view text);

    [[nodiscard]] ClipboardStatus
    readText(std::string& text);

    /// Opens the clipboard, retrying briefly while another application has it
    /// open.
    [[nodiscard]] bool
    openClipboard() noexcept;

private:
    std::shared_ptr<GameWindow> window_;
    HWND windowHandle_ = nullptr;
    std::vector<ClipboardEvent> pendingEvents_;
    std::vector<ClipboardEvent> events_;
    u64 nextRequestID_ = 1;
};

} // namespace pomdog::clipboard::detail::win32
