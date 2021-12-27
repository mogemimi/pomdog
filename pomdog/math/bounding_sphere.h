// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/math/forward_declarations.hpp"
#include "pomdog/math/vector3.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// BoundingSphere is a bounding sphere in 3D space.
class POMDOG_EXPORT BoundingSphere final {
public:
    Vector3 Center;
    float Radius;

public:
    BoundingSphere() noexcept = default;

    BoundingSphere(const Vector3& center, float radius);

    [[nodiscard]] ContainmentType
    Contains(const Vector3& point) const;

    [[nodiscard]] ContainmentType
    Contains(const BoundingBox& box) const;

    [[nodiscard]] ContainmentType
    Contains(const BoundingSphere& sphere) const;

    [[nodiscard]] bool
    Intersects(const BoundingBox& box) const;

    [[nodiscard]] bool
    Intersects(const BoundingSphere& sphere) const;

    [[nodiscard]] PlaneIntersectionType
    Intersects(const Plane& plane) const;

    [[nodiscard]] std::optional<float>
    Intersects(const Ray& ray) const;

    [[nodiscard]] static BoundingSphere
    CreateFromPoints(const Vector3* points, std::size_t pointCount) noexcept;

    [[nodiscard]] static BoundingSphere
    CreateFromPoints(std::function<Vector3(std::size_t)> points, std::size_t pointCount) noexcept;
};

} // namespace pomdog
