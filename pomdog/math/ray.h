// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/vector3.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class BoundingBox;
class BoundingFrustum;
class BoundingSphere;
class Plane;
} // namespace pomdog

namespace pomdog {

/// Ray represents a ray with an origin point and a direction vector in 3D space.
class POMDOG_EXPORT Ray final {
public:
    Vector3 position;
    Vector3 direction;

public:
    Ray() = default;

    Ray(const Vector3& position, const Vector3& direction);

    [[nodiscard]] std::optional<float>
    intersects(const BoundingBox& box) const;

    [[nodiscard]] std::optional<float>
    intersects(const BoundingFrustum& frustum) const;

    [[nodiscard]] std::optional<float>
    intersects(const BoundingSphere& sphere) const;

    [[nodiscard]] std::optional<float>
    intersects(const Plane& plane) const;
};

} // namespace pomdog
