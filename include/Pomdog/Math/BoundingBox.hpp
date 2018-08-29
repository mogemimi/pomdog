// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <array>
#include <optional>

namespace Pomdog {

class POMDOG_EXPORT BoundingBox final {
public:
    Vector3 Min;
    Vector3 Max;

    static constexpr int CornerCount = 8;

public:
    BoundingBox() noexcept = default;

    BoundingBox(const Vector3& min, const Vector3& max);

    bool operator==(const BoundingBox&) const noexcept;
    bool operator!=(const BoundingBox&) const noexcept;

    ContainmentType Contains(const Vector3& point) const;

    ContainmentType Contains(const BoundingBox& box) const;

    ContainmentType Contains(const BoundingSphere& sphere) const;

    bool Intersects(const BoundingBox& box) const;

    bool Intersects(const BoundingSphere& sphere) const;

    PlaneIntersectionType Intersects(const Plane& plane) const;

    std::optional<float> Intersects(const Ray& ray) const;

    std::array<Vector3, CornerCount> GetCorners() const noexcept;
};

} // namespace Pomdog
