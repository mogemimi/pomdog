// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/system_events.h"
#include "pomdog/input/mouse.h"
#include "pomdog/input/mouse_state.h"
#include "pomdog/platform/win32/prerequisites_win32.h"
#include "pomdog/signals/forward_declarations.h"

namespace pomdog::detail::win32 {

class MouseWin32 final : public Mouse {
public:
    explicit MouseWin32(HWND windowHandle);

    MouseState GetState() const override;

    void HandleMessage(const SystemEvent& event);

private:
    HWND windowHandle;
    MouseState previousState;
    MouseState state;
};

void TranslateMouseEvent(HWND windowHandle, const RAWMOUSE& mouse, const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue) noexcept;

} // namespace pomdog::detail::win32
