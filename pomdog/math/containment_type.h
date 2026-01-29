// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog {

/// ContainmentType indicates whether a geometry object contains another one.
enum class ContainmentType : u8 {
    /// The object completely contains the other one.
    Contains,

    /// The object is completely outside the other one.
    Disjoint,

    /// The object partially overlaps the other one.
    Intersects,
};

} // namespace pomdog
