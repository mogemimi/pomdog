// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/math/forward_declarations.hpp"
#include "pomdog/math/vector3.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Ray represents a ray with an origin point and a direction vector in 3D space.
class POMDOG_EXPORT Ray final {
public:
    Vector3 Position;
    Vector3 Direction;

public:
    Ray() = default;

    Ray(const Vector3& position, const Vector3& direction);

    [[nodiscard]] std::optional<float>
    Intersects(const BoundingBox& box) const;

    [[nodiscard]] std::optional<float>
    Intersects(const BoundingFrustum& frustum) const;

    [[nodiscard]] std::optional<float>
    Intersects(const BoundingSphere& sphere) const;

    [[nodiscard]] std::optional<float>
    Intersects(const Plane& plane) const;
};

} // namespace pomdog
