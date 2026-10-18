// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/clipboard/win32/clipboard_service_win32.h"
#include "pomdog/application/game_window.h"
#include "pomdog/experimental/clipboard/line_breaks.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cwchar>
#include <optional>
#include <span>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::clipboard::detail::win32 {
namespace {

/// The number of times to try opening the clipboard while another
/// application has it open, one millisecond apart.
constexpr int openClipboardAttempts = 5;

/// Returns the number of UTF-16 code units that text takes, or nullopt when
/// the conversion fails.
[[nodiscard]] std::optional<int>
countUTF16(std::string_view text) noexcept
{
    if (text.empty()) {
        return 0;
    }
    const auto count = ::MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
    if (count <= 0) {
        return std::nullopt;
    }
    return count;
}

} // namespace

ClipboardServiceWin32::ClipboardServiceWin32(std::shared_ptr<GameWindow> window, HWND windowHandle) noexcept
    : window_(std::move(window))
    , windowHandle_(windowHandle)
{
    POMDOG_ASSERT(window_ != nullptr);
    POMDOG_ASSERT(windowHandle_ != nullptr);
}

ClipboardServiceWin32::~ClipboardServiceWin32() = default;

ClipboardCapabilities ClipboardServiceWin32::getCapabilities() const noexcept
{
    ClipboardCapabilities capabilities;
    capabilities.writeText = FeatureSupport::Available;
    capabilities.readText = FeatureSupport::Available;

    // NOTE: Windows has no paste event; the application reads the clipboard
    // when the user presses the paste shortcut.
    capabilities.pasteEvents = FeatureSupport::Unsupported;
    return capabilities;
}

void ClipboardServiceWin32::update()
{
    events_.clear();
    std::swap(events_, pendingEvents_);
}

std::span<const ClipboardEvent> ClipboardServiceWin32::getEvents() const noexcept
{
    return events_;
}

u64 ClipboardServiceWin32::requestWriteText(std::string_view text)
{
    ClipboardEvent event;
    event.kind = ClipboardEventKind::WriteCompleted;
    event.requestID = nextRequestID_++;
    event.status = writeText(text);
    pendingEvents_.push_back(std::move(event));
    return pendingEvents_.back().requestID;
}

u64 ClipboardServiceWin32::requestReadText()
{
    ClipboardEvent event;
    event.kind = ClipboardEventKind::ReadCompleted;
    event.requestID = nextRequestID_++;
    event.status = readText(event.text);
    pendingEvents_.push_back(std::move(event));
    return pendingEvents_.back().requestID;
}

ClipboardStatus ClipboardServiceWin32::writeText(std::string_view text)
{
    // NOTE: Windows applications expect CR LF line breaks on the clipboard.
    std::string converted;
    appendWithCRLF(text, converted);

    const auto count = countUTF16(converted);
    if (!count) {
        return ClipboardStatus::Failed;
    }
    const auto units = static_cast<std::size_t>(*count);

    // NOTE: The clipboard takes memory from GlobalAlloc() with GMEM_MOVEABLE,
    // ending with a null character.
    auto memory = ::GlobalAlloc(GMEM_MOVEABLE, (units + 1) * sizeof(wchar_t));
    if (memory == nullptr) {
        return ClipboardStatus::Failed;
    }
    auto locked = static_cast<wchar_t*>(::GlobalLock(memory));
    if (locked == nullptr) {
        ::GlobalFree(memory);
        return ClipboardStatus::Failed;
    }
    const std::span<wchar_t> buffer{locked, units + 1};
    if (units > 0) {
        ::MultiByteToWideChar(CP_UTF8, 0, converted.data(), static_cast<int>(converted.size()), buffer.data(), *count);
    }
    buffer[units] = L'\0';
    ::GlobalUnlock(memory);

    if (!openClipboard()) {
        ::GlobalFree(memory);
        return ClipboardStatus::Failed;
    }
    ::EmptyClipboard();
    if (::SetClipboardData(CF_UNICODETEXT, memory) == nullptr) {
        ::CloseClipboard();
        ::GlobalFree(memory);
        return ClipboardStatus::Failed;
    }

    // NOTE: The system owns memory once SetClipboardData() succeeds.
    ::CloseClipboard();
    return ClipboardStatus::Succeeded;
}

ClipboardStatus ClipboardServiceWin32::readText(std::string& text)
{
    text.clear();
    if (::IsClipboardFormatAvailable(CF_UNICODETEXT) == FALSE) {
        return ClipboardStatus::NoText;
    }
    if (!openClipboard()) {
        return ClipboardStatus::Failed;
    }

    auto status = ClipboardStatus::Failed;
    if (auto memory = ::GetClipboardData(CF_UNICODETEXT); memory != nullptr) {
        if (auto locked = static_cast<const wchar_t*>(::GlobalLock(memory)); locked != nullptr) {
            // NOTE: The text ends at its null character, which may come before
            // the end of the memory block.
            const auto capacity = ::GlobalSize(memory) / sizeof(wchar_t);
            const std::span<const wchar_t> units{locked, ::wcsnlen(locked, capacity)};

            std::string utf8;
            if (units.empty()) {
                status = ClipboardStatus::Succeeded;
            }
            else if (const auto bytes = ::WideCharToMultiByte(CP_UTF8, 0, units.data(), static_cast<int>(units.size()), nullptr, 0, nullptr, nullptr); bytes > 0) {
                utf8.resize(static_cast<std::size_t>(bytes));
                ::WideCharToMultiByte(CP_UTF8, 0, units.data(), static_cast<int>(units.size()), utf8.data(), bytes, nullptr, nullptr);
                status = ClipboardStatus::Succeeded;
            }
            ::GlobalUnlock(memory);
            appendWithLF(utf8, text);
        }
    }
    ::CloseClipboard();
    return status;
}

bool ClipboardServiceWin32::openClipboard() noexcept
{
    for (int attempt = 0; attempt < openClipboardAttempts; ++attempt) {
        if (::OpenClipboard(windowHandle_) != FALSE) {
            return true;
        }
        ::Sleep(1);
    }
    return false;
}

} // namespace pomdog::clipboard::detail::win32
