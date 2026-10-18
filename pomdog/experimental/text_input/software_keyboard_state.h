// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/f32_rect2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::textinput {

/// Describes whether the software keyboard shows.
enum class SoftwareKeyboardVisibility : u8 {
    /// The platform does not tell, such as a browser on a phone.
    Unknown,

    Hidden,

    Visible,
};

/// Describes the software keyboard.
struct SoftwareKeyboardState final {
    SoftwareKeyboardVisibility visibility = SoftwareKeyboardVisibility::Unknown;

    /// The area the keyboard covers, in the window's logical pixels, when it
    /// shows and the platform reports it.
    std::optional<F32Rect2D> area;
};

} // namespace pomdog::textinput
