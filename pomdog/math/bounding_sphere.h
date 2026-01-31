// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/vector3.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class BoundingBox;
class Plane;
class Ray;
enum class ContainmentType : u8;
enum class PlaneIntersectionType : u8;
} // namespace pomdog

namespace pomdog {

/// BoundingSphere is a bounding sphere in 3D space.
class POMDOG_EXPORT BoundingSphere final {
public:
    Vector3 center;
    float radius;

public:
    BoundingSphere() noexcept = default;

    BoundingSphere(const Vector3& center, float radius) noexcept;

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

    [[nodiscard]] static BoundingSphere
    createFromPoints(const Vector3* points, i32 pointCount) noexcept;

    [[nodiscard]] static BoundingSphere
    createFromPoints(std::function<Vector3(i32 index)> points, i32 pointCount) noexcept;
};

} // namespace pomdog
