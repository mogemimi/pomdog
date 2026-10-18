// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/clipboard/platform_clipboard.h"
#include "pomdog/application/game_host.h"
#include "pomdog/basic/platform.h"
#include "pomdog/experimental/clipboard/clipboard_service.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

#if defined(POMDOG_PLATFORM_WIN32)
#include "pomdog/application/win32/game_window_win32.h"
#include "pomdog/experimental/clipboard/win32/clipboard_service_win32.h"
#elif defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include "pomdog/experimental/clipboard/emscripten/clipboard_service_emscripten.h"
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::clipboard {
namespace {

#if !defined(POMDOG_PLATFORM_WIN32) && !defined(POMDOG_PLATFORM_EMSCRIPTEN)
/// Ends every request with Unsupported, for platforms without a backend.
class ClipboardServiceUnimplemented final : public ClipboardService {
public:
    [[nodiscard]] ClipboardCapabilities
    getCapabilities() const noexcept override
    {
        return ClipboardCapabilities{};
    }

    void
    update() override
    {
        events_.clear();
        std::swap(events_, pendingEvents_);
    }

    [[nodiscard]] std::span<const ClipboardEvent>
    getEvents() const noexcept override
    {
        return events_;
    }

    u64
    requestWriteText([[maybe_unused]] std::string_view text) override
    {
        return push(ClipboardEventKind::WriteCompleted);
    }

    u64
    requestReadText() override
    {
        return push(ClipboardEventKind::ReadCompleted);
    }

private:
    u64
    push(ClipboardEventKind kind)
    {
        ClipboardEvent event;
        event.kind = kind;
        event.requestID = nextRequestID_++;
        event.status = ClipboardStatus::Unsupported;
        pendingEvents_.push_back(std::move(event));
        return pendingEvents_.back().requestID;
    }

private:
    std::vector<ClipboardEvent> pendingEvents_;
    std::vector<ClipboardEvent> events_;
    u64 nextRequestID_ = 1;
};
#endif

} // namespace

std::tuple<std::shared_ptr<ClipboardService>, std::unique_ptr<Error>>
createClipboardService([[maybe_unused]] const std::shared_ptr<GameHost>& gameHost)
{
    // NOTE: Only Windows needs the host today. X11 and Wayland backends will
    // need its connection and window too, for selections and input serials.
    POMDOG_ASSERT(gameHost != nullptr);

#if defined(POMDOG_PLATFORM_WIN32)
    auto window = std::dynamic_pointer_cast<pomdog::detail::win32::GameWindowWin32>(gameHost->getWindow());
    if (window == nullptr) {
        return {nullptr, errors::make("the game host has no Win32 window")};
    }
    return {std::make_shared<detail::win32::ClipboardServiceWin32>(window, window->getNativeWindowHandle()), nullptr};
#elif defined(POMDOG_PLATFORM_EMSCRIPTEN)
    auto service = std::make_shared<detail::emscripten::ClipboardServiceEmscripten>();
    if (auto err = service->initialize(); err != nullptr) {
        return {nullptr, errors::wrap(std::move(err), "ClipboardServiceEmscripten::initialize() failed")};
    }
    return {std::move(service), nullptr};
#else
    return {std::make_shared<ClipboardServiceUnimplemented>(), nullptr};
#endif
}

} // namespace pomdog::clipboard
