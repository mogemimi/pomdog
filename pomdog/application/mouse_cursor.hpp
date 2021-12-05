// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

/// Avaliable system provided cursor assets.
/// @todo Add preview images.
enum class MouseCursor : std::uint8_t {
    Arrow,
    IBeam,
    PointingHand,
    ResizeVertical,
    ResizeHorizontal,
};

} // namespace Pomdog
