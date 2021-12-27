// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Avaliable system provided cursor assets.
/// @todo Add preview images.
enum class MouseCursor : std::uint8_t {
    Arrow,
    IBeam,
    PointingHand,
    ResizeVertical,
    ResizeHorizontal,
};

} // namespace pomdog
