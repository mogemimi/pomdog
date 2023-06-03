// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/system_events.h"
#include "pomdog/input/mouse.h"
#include "pomdog/input/mouse_state.h"
#include "pomdog/platform/win32/prerequisites_win32.h"
#include "pomdog/signals/forward_declarations.h"

namespace pomdog::detail::win32 {

class MouseWin32 final : public Mouse {
private:
    HWND windowHandle_;
    MouseState previousState_;
    MouseState state_;

public:
    explicit MouseWin32(HWND windowHandle);

    [[nodiscard]] MouseState
    getState() const override;

    void handleMessage(const SystemEvent& event);
};

void translateMouseEvent(HWND windowHandle, const RAWMOUSE& mouse, const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue) noexcept;

} // namespace pomdog::detail::win32
