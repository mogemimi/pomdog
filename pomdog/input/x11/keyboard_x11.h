// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/keys.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <X11/Xlib.h>
#include <array>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
class KeyboardImpl;
} // namespace pomdog::detail

namespace pomdog::detail::x11 {

class KeyboardX11 final {
private:
    std::array<Keys, 256> mappedKeys_;
    std::shared_ptr<KeyboardImpl> impl_;

public:
    KeyboardX11(::Display* display, const std::shared_ptr<KeyboardImpl>& impl);

    void handleEvent(XEvent& event, ::XIC inputContext);
};

} // namespace pomdog::detail::x11
