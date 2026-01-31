// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/vector2.h"

namespace pomdog {
class BoundingBox2D;
enum class ContainmentType : u8;
} // namespace pomdog

namespace pomdog {

/// BoundingCircle is a bounding circle in 2D space.
class POMDOG_EXPORT BoundingCircle final {
public:
    Vector2 center;
    float radius;

public:
    BoundingCircle() noexcept = default;

    BoundingCircle(const Vector2& center, float radius) noexcept;

    [[nodiscard]] ContainmentType
    contains(const Vector2& box) const noexcept;

    [[nodiscard]] ContainmentType
    contains(const BoundingBox2D& box) const noexcept;

    [[nodiscard]] ContainmentType
    contains(const BoundingCircle& circle) const noexcept;

    [[nodiscard]] bool
    intersects(const BoundingBox2D& box) const noexcept;

    [[nodiscard]] bool
    intersects(const BoundingCircle& circle) const noexcept;
};

} // namespace pomdog
