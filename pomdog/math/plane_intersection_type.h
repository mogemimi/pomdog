// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog {

/// PlaneIntersectionType indicates whether a geometry object intersects a plane.
enum class PlaneIntersectionType : u8 {
    //// The object is in front of the plane.
    Front,

    //// The object is behind the plane.
    Back,

    //// The object intersects the plane.
    Intersecting,
};

} // namespace pomdog
