// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/math/forward_declarations.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// BoundingBox2D is an axis-aligned bounding box in 2D space.
class POMDOG_EXPORT BoundingBox2D final {
public:
    Vector2 min;
    Vector2 max;

    static constexpr int CornerCount = 4;

public:
    BoundingBox2D() noexcept = default;

    BoundingBox2D(const Vector2& min, const Vector2& max) noexcept;

    [[nodiscard]] ContainmentType
    contains(const Vector2& point) const noexcept;

    [[nodiscard]] ContainmentType
    contains(const BoundingBox2D& box) const noexcept;

    [[nodiscard]] ContainmentType
    contains(const BoundingCircle& circle) const noexcept;

    [[nodiscard]] bool
    intersects(const BoundingBox2D& box) const noexcept;

    [[nodiscard]] bool
    intersects(const BoundingCircle& circle) const noexcept;

    [[nodiscard]] std::array<Vector2, CornerCount>
    getCorners() const noexcept;
};

} // namespace pomdog
