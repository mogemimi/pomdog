// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {

/// Measures text for layout.
///
/// A renderer implements this interface so that layout and drawing use the
/// same font. Tests can supply a fixed-width implementation.
class TextMeasurer {
public:
    virtual ~TextMeasurer();

    /// Returns the width in logical pixels of UTF-8 text drawn at fontSize.
    [[nodiscard]] virtual f32
    measureWidth(std::string_view text, f32 fontSize) = 0;
};

} // namespace pomdog::devgui
