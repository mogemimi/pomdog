// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/vector_xz.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class BoundingCircleXZ;
enum class ContainmentType : u8;
} // namespace pomdog

namespace pomdog {

/// BoundingBoxXZ is an axis-aligned bounding box in 2D XZ space.
class POMDOG_EXPORT BoundingBoxXZ final {
public:
    VectorXZ min;
    VectorXZ max;

    static constexpr i32 CornerCount = 4;

public:
    BoundingBoxXZ() noexcept;

    BoundingBoxXZ(const VectorXZ& min, const VectorXZ& max) noexcept;

    /// Returns true if the specified point is inside the bounding box.
    [[nodiscard]] bool
    contains(const VectorXZ& point) const noexcept;

    /// Determines whether the bounding box contains the specified box.
    [[nodiscard]] ContainmentType
    contains(const BoundingBoxXZ& box) const noexcept;

    /// Determines whether the bounding box contains the specified circle.
    [[nodiscard]] ContainmentType
    contains(const BoundingCircleXZ& circle) const noexcept;

    /// Determines whether the bounding box intersects the specified box.
    [[nodiscard]] bool
    intersects(const BoundingBoxXZ& box) const noexcept;

    /// Determines whether the bounding box intersects the specified circle.
    [[nodiscard]] bool
    intersects(const BoundingCircleXZ& circle) const noexcept;

    /// Returns the corners of the bounding box.
    [[nodiscard]] std::array<VectorXZ, CornerCount>
    getCorners() const noexcept;
};

} // namespace pomdog
