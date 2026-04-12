// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/system_events.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/backends/mouse_state.h"
#include "pomdog/input/mouse.h"
#include "pomdog/platform/win32/prerequisites_win32.h"
#include "pomdog/signals/event_queue.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::win32 {

class MouseWin32 final : public Mouse {
private:
    HWND windowHandle_;
    MouseState state_;

public:
    explicit MouseWin32(HWND windowHandle);

    [[nodiscard]] Point2D
    getPosition() const noexcept override;

    [[nodiscard]] bool
    isButtonDown(MouseButtons button) const noexcept override;

    [[nodiscard]] i32
    getScrollX() const noexcept override;

    [[nodiscard]] i32
    getScrollY() const noexcept override;

    [[nodiscard]] bool
    isPresent() const noexcept override;

    void handleMessage(const SystemEvent& event);

    void clearAllButtons() noexcept;
};

void translateMouseEvent(HWND windowHandle, const RAWMOUSE& mouse, const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue) noexcept;

} // namespace pomdog::detail::win32
