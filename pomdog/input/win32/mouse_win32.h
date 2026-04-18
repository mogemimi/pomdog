// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/platform/win32/prerequisites_win32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
class SystemEvent;
class SystemEventQueue;
class MouseImpl;
} // namespace pomdog::detail

namespace pomdog::detail::win32 {

class MouseWin32 final {
private:
    std::shared_ptr<MouseImpl> impl_;

public:
    MouseWin32(HWND windowHandle, const std::shared_ptr<MouseImpl>& impl);

    void handleMessage(const SystemEvent& event);
};

void translateMouseEvent(HWND windowHandle, const RAWMOUSE& mouse, const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept;

} // namespace pomdog::detail::win32
