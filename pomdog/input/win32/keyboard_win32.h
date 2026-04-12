// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/platform/win32/prerequisites_win32.h"
#include "pomdog/signals/event_queue.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
class SystemEvent;
class KeyboardImpl;
} // namespace pomdog::detail

namespace pomdog::detail::win32 {

class KeyboardWin32 final {
private:
    std::shared_ptr<KeyboardImpl> impl_;

public:
    explicit KeyboardWin32(const std::shared_ptr<KeyboardImpl>& impl);

    void handleMessage(const SystemEvent& event);
};

void translateKeyboardEvent(const RAWKEYBOARD& keyboard, const std::shared_ptr<EventQueue<SystemEvent>>& eventQueue) noexcept;

} // namespace pomdog::detail::win32
