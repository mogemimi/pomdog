// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class BoundingCircle;
enum class ContainmentType : u8;
} // namespace pomdog

namespace pomdog {

/// BoundingBox2D is an axis-aligned bounding box in 2D space.
class POMDOG_EXPORT BoundingBox2D final {
public:
    Vector2 min;
    Vector2 max;

    static constexpr i32 CornerCount = 4;

public:
    BoundingBox2D() noexcept;

    BoundingBox2D(const Vector2& min, const Vector2& max) noexcept;

    /// Returns true if the specified point is inside the bounding box.
    [[nodiscard]] bool
    contains(const Vector2& point) const noexcept;

    /// Determines whether the bounding box contains the specified box.
    [[nodiscard]] ContainmentType
    contains(const BoundingBox2D& box) const noexcept;

    /// Determines whether the bounding box contains the specified circle.
    [[nodiscard]] ContainmentType
    contains(const BoundingCircle& circle) const noexcept;

    /// Determines whether the bounding box intersects the specified box.
    [[nodiscard]] bool
    intersects(const BoundingBox2D& box) const noexcept;

    /// Determines whether the bounding box intersects the specified circle.
    [[nodiscard]] bool
    intersects(const BoundingCircle& circle) const noexcept;

    /// Returns the corners of the bounding box.
    [[nodiscard]] std::array<Vector2, CornerCount>
    getCorners() const noexcept;
};

} // namespace pomdog
