// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

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
