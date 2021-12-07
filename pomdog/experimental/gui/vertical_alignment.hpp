// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::GUI {

enum class VerticalAlignment : std::uint8_t {
    //Bottom,
    //Center,
    Stretch,
    Top,
};

} // namespace Pomdog::GUI
