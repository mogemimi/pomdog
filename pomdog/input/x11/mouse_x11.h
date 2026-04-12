// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/mouse_buttons.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <X11/Xlib.h>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
class MouseImpl;
} // namespace pomdog::detail

namespace pomdog::detail::x11 {

class MouseX11 final {
private:
    std::shared_ptr<MouseImpl> impl_;

public:
    explicit MouseX11(const std::shared_ptr<MouseImpl>& impl);

    void handleEvent(XEvent& event);
};

} // namespace pomdog::detail::x11
