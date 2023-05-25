// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/math/forward_declarations.h"
#include "pomdog/math/vector3.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// BoundingBox is an axis-aligned bounding box in 3D space.
class POMDOG_EXPORT BoundingBox final {
public:
    Vector3 min;
    Vector3 max;

    static constexpr int CornerCount = 8;

public:
    BoundingBox() noexcept = default;

    BoundingBox(const Vector3& min, const Vector3& max) noexcept;

    [[nodiscard]] ContainmentType
    contains(const Vector3& point) const noexcept;

    [[nodiscard]] ContainmentType
    contains(const BoundingBox& box) const noexcept;

    [[nodiscard]] ContainmentType
    contains(const BoundingSphere& sphere) const noexcept;

    [[nodiscard]] bool
    intersects(const BoundingBox& box) const noexcept;

    [[nodiscard]] bool
    intersects(const BoundingSphere& sphere) const noexcept;

    [[nodiscard]] PlaneIntersectionType
    intersects(const Plane& plane) const noexcept;

    [[nodiscard]] std::optional<float>
    intersects(const Ray& ray) const noexcept;

    [[nodiscard]] std::array<Vector3, CornerCount>
    getCorners() const noexcept;
};

} // namespace pomdog
