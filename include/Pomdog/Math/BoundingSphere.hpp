// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

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

    bool operator==(const BoundingSphere&) const noexcept;
    bool operator!=(const BoundingSphere&) const noexcept;

    ContainmentType Contains(const Vector3& point) const;

    ContainmentType Contains(const BoundingBox& box) const;

    ContainmentType Contains(const BoundingSphere& sphere) const;

    bool Intersects(const BoundingBox& box) const;

    bool Intersects(const BoundingSphere& sphere) const;

    PlaneIntersectionType Intersects(const Plane& plane) const;

    std::optional<float> Intersects(const Ray& ray) const;
};

} // namespace Pomdog
