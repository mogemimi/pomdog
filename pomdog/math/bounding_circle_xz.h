// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/vector_xz.h"

namespace pomdog {
class BoundingBoxXZ;
enum class ContainmentType : u8;
} // namespace pomdog

namespace pomdog {

/// BoundingCircleXZ is a bounding circle in 2D XZ space.
class POMDOG_EXPORT BoundingCircleXZ final {
public:
    VectorXZ center;
    f32 radius;

public:
    BoundingCircleXZ() noexcept;

    BoundingCircleXZ(const VectorXZ& center, f32 radius) noexcept;

    /// Determines whether the circle contains the specified point.
    [[nodiscard]] ContainmentType
    contains(const VectorXZ& point) const noexcept;

    /// Determines whether the circle contains the specified box.
    [[nodiscard]] ContainmentType
    contains(const BoundingBoxXZ& box) const noexcept;

    /// Determines whether the circle contains the specified circle.
    [[nodiscard]] ContainmentType
    contains(const BoundingCircleXZ& circle) const noexcept;

    /// Determines whether the circle intersects the specified box.
    [[nodiscard]] bool
    intersects(const BoundingBoxXZ& box) const noexcept;

    /// Determines whether the circle intersects the specified circle.
    [[nodiscard]] bool
    intersects(const BoundingCircleXZ& circle) const noexcept;
};

} // namespace pomdog
