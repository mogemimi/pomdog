// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <optional>

namespace Pomdog {

class POMDOG_EXPORT BoundingSphere final {
public:
    Vector3 Center;
    float Radius;

public:
    BoundingSphere() noexcept = default;

    BoundingSphere(const Vector3& center, float radius);

    BoundingSphere(const BoundingSphere&) noexcept = default;
    BoundingSphere(BoundingSphere&&) noexcept = default;

    BoundingSphere& operator=(const BoundingSphere&) noexcept = default;
    BoundingSphere& operator=(BoundingSphere&&) noexcept = default;

    [[nodiscard]] bool operator==(const BoundingSphere&) const noexcept;
    [[nodiscard]] bool operator!=(const BoundingSphere&) const noexcept;

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
};

} // namespace Pomdog
