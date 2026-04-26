// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/win32/windows_headers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
class SystemEvent;
class SystemEventQueue;
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

void translateKeyboardEvent(const RAWKEYBOARD& keyboard, const std::shared_ptr<SystemEventQueue>& eventQueue) noexcept;

} // namespace pomdog::detail::win32
